#!/usr/bin/env python3
"""
xbld_sign.py - Firmware image signing tool for xBLD bootloader.

Creates a signed hex file with a 512-byte xBLD_ImageHeader_t prepended.
Compatible with xbld_image.h / xbld_image.c verification.

Header layout (512 bytes total):
  Offset  Size  Field
  0       4     magic          = 0xB007C0DE
  4       2     version_major
  6       2     version_minor
  8       2     version_patch
  10      2     hdr_version    = 1
  12      4     image_size     (firmware only, not including header)
  16      4     load_addr      (= hdr_addr + 512)
  20      4     entry_addr     (= load_addr; jump_to_app reads SP/Reset from vector table)
  24      4     image_crc32    (zlib.crc32 of firmware bytes)
  28      4     hdr_crc32      (zlib.crc32 of header with this field = 0)
  32      4     timestamp      (unix time)
  36      4     build_number
  40      1     hw_id
  41      1     flags
  42      6     reserved
  48      464   padding (0x00)
  ----
  512 bytes total

CRC32: zlib.crc32 (ISO-HDLC, poly 0xEDB88320 reflected)

Flash layout (SAMV71Q21B, 2 MB):
  0x00400000  Bootloader  128 KB  (sector-aligned, never erased by xBLD)
  0x00420000  Slot 0 hdr  512 B   --slot 0  (default)
  0x00420200  Slot 0 app  1023 KB
  0x00520000  Slot 1 hdr  512 B   --slot 1
  0x00520200  Slot 1 app  895 KB

Output filename format (from outputconfig.txt + buildtracking.txt):
  slot 0 (main):   [PREFIX_PROJECT]_[BOARD_NAME]_hw[HW_VERSION]_fw[build]_[epoch].hex
  slot 1 (backup): [PREFIX_PROJECT]_[BOARD_NAME]_hw[HW_VERSION]_fw[build]_[epoch]_s1.hex
  e.g. NANORACK_EXP_hw110_fw656_1778172334.hex
       NANORACK_EXP_hw110_fw656_1778172334_s1.hex

Usage:
  python xbld_sign.py input.hex
  python xbld_sign.py input.hex --slot 0          # slot 0 (default)
  python xbld_sign.py input.hex --slot 1          # slot 1 / backup
  python xbld_sign.py input.hex --hw-id 1
  python xbld_sign.py input.hex --output my_custom_name.hex
  python xbld_sign.py input.hex --version-file path/version.txt
  python xbld_sign.py input.hex --output-config path/outputconfig.txt
  python xbld_sign.py input.hex --build-tracking path/buildtracking.txt
"""

import struct
import zlib
import time
import sys
import os
import argparse

try:
    import intelhex
except ImportError:
    print("Error: 'intelhex' package required. Install with: pip install intelhex")
    sys.exit(1)

# ---------------------------------------------------------------------------
# Script location — all paths resolve relative to xbld_sign.py itself
# ---------------------------------------------------------------------------
SCRIPT_DIR   = os.path.dirname(os.path.abspath(__file__))
FIRMWARE_DIR = os.path.normpath(os.path.join(SCRIPT_DIR, '..', 'firmware'))
ORIGINAL_DIR = os.path.join(FIRMWARE_DIR, 'original')
SIGNED_DIR   = os.path.join(FIRMWARE_DIR, 'signed')

IMAGE_MAGIC        = 0xB007C0DE
IMAGE_HEADER_SIZE  = 512
IMAGE_HDR_VERSION  = 1
MAX_FLASH_RANGE    = 2000 * 1024   # 2 MB sanity cap

# ---------------------------------------------------------------------------
# Slot definitions — must match xbld_config.h
# ---------------------------------------------------------------------------
SLOTS = {
    0: {
        'hdr_addr': 0x00420000,
        'max_size': 0x000FFE00,   # 1023 KB  (up to 0x0051FFFF)
        'label':    'main',
    },
    1: {
        'hdr_addr': 0x00520000,
        'max_size': 0x000DFE00,   # 895 KB   (up to 0x005FFFFF)
        'label':    'backup',
    },
}

HW_IDS = {
    'samv71':    1,
    'stm32f4':   2,
    'stm32h7':   3,
    'tiva_tm4c': 4,
}

# ---------------------------------------------------------------------------
# Default content for auto-created config files
# ---------------------------------------------------------------------------
DEFAULT_OUTPUT_CONFIG = """\
# xbld_sign output filename configuration
# Output format: [PREFIX_PROJECT]_[BOARD_NAME]_hw[HW_VERSION]_fw[build]_[epoch].hex (slot 0)
#            or: ..._s1.hex  (slot 1+)

PREFIX_PROJECT=PROJECT
BOARD_NAME=BOARD
HW_VERSION=100
"""

DEFAULT_BUILD_TRACKING = """\
# xbld_sign build counter - do not delete
# Each successful signing run increments this number by 1
# You can manually edit this number at any time
0
"""


def crc32(data: bytes) -> int:
    """CRC32 compatible with zlib / xBLD firmware."""
    return zlib.crc32(data) & 0xFFFFFFFF


# ---------------------------------------------------------------------------
# Output config helpers
# ---------------------------------------------------------------------------

def read_output_config(path: str) -> dict:
    defaults = {
        'PREFIX_PROJECT': 'PROJECT',
        'BOARD_NAME':     'BOARD',
        'HW_VERSION':     '100',
    }
    if not os.path.exists(path):
        print(f"  [INFO] {path} not found - creating with defaults")
        with open(path, 'w') as f:
            f.write(DEFAULT_OUTPUT_CONFIG)
        return defaults

    cfg = dict(defaults)
    try:
        with open(path, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                if '=' in line:
                    k, v = line.split('=', 1)
                    k, v = k.strip(), v.strip()
                    if k in cfg:
                        cfg[k] = v
    except Exception as e:
        print(f"  [WARN] Read output config failed: {e}")
    return cfg


def read_build_count(path: str) -> int:
    if not os.path.exists(path):
        print(f"  [INFO] {path} not found - creating, starting at build 0")
        with open(path, 'w') as f:
            f.write(DEFAULT_BUILD_TRACKING)
        return 0

    try:
        with open(path, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    return int(line)
    except Exception as e:
        print(f"  [WARN] Read build tracking failed: {e}")
    return 0


def write_build_count(path: str, count: int):
    lines = []
    if os.path.exists(path):
        with open(path, 'r') as f:
            for line in f:
                if line.strip().startswith('#') or not line.strip():
                    lines.append(line)
                else:
                    break
    lines.append(f"{count}\n")
    with open(path, 'w') as f:
        f.writelines(lines)


def make_output_filename(cfg: dict, build_count: int, epoch: int,
                         slot: int, output_dir: str = '.') -> str:
    """
    Compose output filename:
      slot 0 (main):   [PREFIX_PROJECT]_[BOARD_NAME]_hw[HW_VERSION]_fw[build]_[epoch].hex
      slot 1+ (backup): ... _s[slot].hex
    """
    slot_labels = {0: "", 1: "_backup"}
    slot_suffix = slot_labels.get(slot, f"_s{slot}")
    name = (f"{cfg['PREFIX_PROJECT']}_{cfg['BOARD_NAME']}"
            f"_hw{cfg['HW_VERSION']}"
            f"_fw{build_count}"
            f"_{epoch}"
            f"{slot_suffix}.hex")
    return os.path.join(output_dir, name)


# ---------------------------------------------------------------------------
# Version helpers
# ---------------------------------------------------------------------------

def read_version(path: str) -> dict:
    ver = {'MAJOR': 1, 'MINOR': 0, 'PATCH': 0, 'BUILD': 0}
    try:
        if os.path.exists(path):
            with open(path, 'r') as f:
                for line in f:
                    line = line.strip()
                    if '=' in line and not line.startswith('#'):
                        k, v = line.split('=', 1)
                        k, v = k.strip(), v.strip()
                        if k in ver:
                            ver[k] = int(v)
    except Exception as e:
        print(f"  [WARN] Read version failed: {e}")
    return ver


def write_version(path: str, ver: dict):
    lines = []
    if os.path.exists(path):
        with open(path, 'r') as f:
            for line in f:
                s = line.strip()
                if '=' in s and not s.startswith('#'):
                    k = s.split('=', 1)[0].strip()
                    if k in ver:
                        line = f"{k}={ver[k]}\n"
                lines.append(line)
    else:
        for k, v in ver.items():
            lines.append(f"{k}={v}\n")
    with open(path, 'w') as f:
        f.writelines(lines)


# ---------------------------------------------------------------------------
# Build header
# ---------------------------------------------------------------------------

def build_header(ver: dict, image_size: int, load_addr: int,
                 image_crc: int, hw_id: int, flags: int = 0) -> bytes:
    """
    Build a 512-byte xBLD_ImageHeader_t.

    entry_addr = load_addr (= hdr_addr + 512).
    port_jump_to_app() receives entry_addr as app_vector_addr and reads:
      SP           = *(app_vector_addr + 0)
      Reset_Handler = *(app_vector_addr + 4)
    so entry_addr must point to the ARM vector table, which is at load_addr.
    """
    hdr = bytearray(IMAGE_HEADER_SIZE)

    struct.pack_into('<I', hdr,  0, IMAGE_MAGIC)
    struct.pack_into('<H', hdr,  4, ver['MAJOR'])
    struct.pack_into('<H', hdr,  6, ver['MINOR'])
    struct.pack_into('<H', hdr,  8, ver['PATCH'])
    struct.pack_into('<H', hdr, 10, IMAGE_HDR_VERSION)
    struct.pack_into('<I', hdr, 12, image_size)
    struct.pack_into('<I', hdr, 16, load_addr)
    struct.pack_into('<I', hdr, 20, load_addr)   # entry_addr = vector table base
    struct.pack_into('<I', hdr, 24, image_crc)
    struct.pack_into('<I', hdr, 28, 0)           # hdr_crc32 placeholder
    struct.pack_into('<I', hdr, 32, int(time.time()))
    struct.pack_into('<I', hdr, 36, ver.get('BUILD', 0))
    struct.pack_into('<B', hdr, 40, hw_id)
    struct.pack_into('<B', hdr, 41, flags)
    # bytes 42..47 = reserved (0)
    # bytes 48..511 = padding  (0)

    h_crc = crc32(bytes(hdr))
    struct.pack_into('<I', hdr, 28, h_crc)

    return bytes(hdr)


# ---------------------------------------------------------------------------
# Main signing logic
# ---------------------------------------------------------------------------

def sign_hex(args):
    print("=" * 60)
    print("  xbld_sign.py - xBLD Firmware Signing Tool")
    print("=" * 60)

    epoch_now = int(time.time())

    # -- Resolve slot ---------------------------------------------------------
    slot_num = args.slot
    if slot_num not in SLOTS:
        print(f"  [ERROR] Unknown slot {slot_num}. Valid: {list(SLOTS.keys())}")
        sys.exit(1)

    slot      = SLOTS[slot_num]
    hdr_addr  = args.hdr_addr if args.hdr_addr is not None else slot['hdr_addr']
    load_addr = hdr_addr + IMAGE_HEADER_SIZE
    max_size  = slot['max_size']
    flash_end = load_addr + max_size

    print(f"  Slot    : {slot_num} ({slot['label']})")
    print(f"  Hdr addr: 0x{hdr_addr:08X}")
    print(f"  App addr: 0x{load_addr:08X}  max {max_size // 1024} KB")

    # -- Version --------------------------------------------------------------
    ver = read_version(args.version_file)
    print(f"  Version : {ver['MAJOR']}.{ver['MINOR']}.{ver['PATCH']} "
          f"Build #{ver['BUILD']}")

    # -- Output config + build tracking ---------------------------------------
    out_cfg     = read_output_config(args.output_config)
    build_count = read_build_count(args.build_tracking)

    os.makedirs(ORIGINAL_DIR, exist_ok=True)
    os.makedirs(SIGNED_DIR,   exist_ok=True)

    if args.output:
        output_path = args.output
    else:
        output_path = make_output_filename(out_cfg, build_count, epoch_now,
                                           slot_num, SIGNED_DIR)
    print(f"  Output  : {os.path.basename(output_path)}")
    print(f"  Config  : prefix={out_cfg['PREFIX_PROJECT']}  "
          f"board={out_cfg['BOARD_NAME']}  "
          f"hw={out_cfg['HW_VERSION']}  "
          f"fw_build={build_count}")

    # -- Load input hex -------------------------------------------------------
    print(f"  Loading : {args.input}")
    ih = intelhex.IntelHex(args.input)

    import shutil
    orig_dest = os.path.join(ORIGINAL_DIR, os.path.basename(args.input))
    shutil.copy2(args.input, orig_dest)
    print(f"  Original: {orig_dest}")

    # -- Find image extent ----------------------------------------------------
    actual_max = 0
    for s_start, s_end in ih.segments():
        if s_start >= load_addr and s_end <= flash_end:
            actual_max = max(actual_max, s_end)

    if actual_max == 0:
        # Fallback: use whatever the hex contains, warn if it looks wrong
        actual_max = ih.maxaddr() + 1
        print(f"  [WARN] No segments found in slot range; using hex maxaddr "
              f"0x{actual_max:08X}")

    image_size = actual_max - load_addr
    if image_size <= 0:
        print(f"  [ERROR] No valid image data found at 0x{load_addr:08X}")
        sys.exit(1)

    if image_size > max_size:
        print(f"  [ERROR] Image too large: {image_size} B > slot max {max_size} B")
        sys.exit(1)

    print(f"  Image   : {image_size:,} bytes "
          f"(0x{load_addr:08X} - 0x{actual_max - 1:08X})")

    # -- CRC ------------------------------------------------------------------
    image_bytes = bytes(ih.tobinarray(start=load_addr, size=image_size))
    img_crc     = crc32(image_bytes)
    print(f"  Img CRC : 0x{img_crc:08X}")

    # -- Build header ---------------------------------------------------------
    hw_id = args.hw_id
    if isinstance(hw_id, str):
        hw_id = HW_IDS.get(hw_id.lower(), 1)

    header = build_header(ver, image_size, load_addr, img_crc, hw_id)
    h_crc  = struct.unpack_from('<I', header, 28)[0]
    print(f"  Hdr CRC : 0x{h_crc:08X}  |  HW ID: {hw_id}")

    # -- Build output hex -----------------------------------------------------
    print("  Building output hex...")
    out_ih = intelhex.IntelHex()
    out_ih.frombytes(header,      offset=hdr_addr)
    out_ih.frombytes(image_bytes, offset=load_addr)

    # Merge any other segments inside the slot range (ignore overlaps)
    for s_start, s_end in ih.segments():
        if s_start >= hdr_addr and s_end <= flash_end:
            out_ih.merge(ih[s_start:s_end], overlap='ignore')

    # -- Write ----------------------------------------------------------------
    out_ih.write_hex_file(output_path)
    out_size = os.path.getsize(output_path)
    print(f"  Saved   : {output_path} ({out_size:,} bytes)")

    # -- Increment build counter ----------------------------------------------
    if not args.no_increment:
        new_count = build_count + 1
        write_build_count(args.build_tracking, new_count)
        print(f"  Build # : {build_count} -> {new_count}  "
              f"(saved to {args.build_tracking})")
        ver['BUILD'] += 1
        write_version(args.version_file, ver)

    # -- Generate C header ----------------------------------------------------
    if args.gen_header:
        ver_dir = os.path.dirname(os.path.abspath(args.version_file))
        h_path  = os.path.join(ver_dir, 'app_version.h')
        with open(h_path, 'w') as f:
            f.write("#ifndef APP_VERSION_H\n#define APP_VERSION_H\n\n")
            f.write(f'#define APP_VERSION_MAJOR  {ver["MAJOR"]}\n')
            f.write(f'#define APP_VERSION_MINOR  {ver["MINOR"]}\n')
            f.write(f'#define APP_VERSION_PATCH  {ver["PATCH"]}\n')
            f.write(f'#define APP_VERSION_STR    '
                    f'"{ver["MAJOR"]}.{ver["MINOR"]}.{ver["PATCH"]}"\n')
            f.write(f'#define APP_BUILD_NUMBER   {build_count}\n')
            f.write(f'\n#endif /* APP_VERSION_H */\n')
        print(f"  C hdr   : {h_path}")

    print("=" * 60)
    print(f"  DONE  slot={slot_num} ({slot['label']})  ->  "
          f"{os.path.basename(output_path)}")
    print("=" * 60)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def parse_args():
    p = argparse.ArgumentParser(
        description='xBLD image signing tool — prepend 512 B header to hex file')

    p.add_argument('input',
                   help='Input hex file (unsigned)')
    p.add_argument('output', nargs='?', default=None,
                   help='Output hex path (optional; auto-generated if omitted)')

    # Slot / address
    p.add_argument('--slot', type=int, default=0, choices=[0, 1],
                   help='Target flash slot: 0=main (0x00420000), '
                        '1=backup (0x00520000)  [default: 0]')
    p.add_argument('--hdr-addr', type=lambda x: int(x, 0), default=None,
                   help='Override header address (default: from --slot). '
                        'Use only when targeting a custom layout.')

    # Hardware
    p.add_argument('--hw-id', default=1,
                   help='Hardware ID: integer or name '
                        '(samv71, stm32f4, stm32h7, tiva_tm4c)  [default: 1]')

    # Config files
    p.add_argument('--output-config',
                   default=os.path.join(SCRIPT_DIR, 'outputconfig.txt'),
                   help='Output filename config (auto-created if missing)')
    p.add_argument('--build-tracking',
                   default=os.path.join(SCRIPT_DIR, 'buildtracking.txt'),
                   help='Build counter file (auto-created if missing)')
    p.add_argument('--version-file',
                   default=os.path.join(SCRIPT_DIR, 'version.txt'),
                   help='Firmware version file  [default: version.txt]')

    # Behaviour
    p.add_argument('--no-increment', action='store_true',
                   help='Do not increment build counter or version BUILD field')
    p.add_argument('--gen-header', action='store_true', default=True,
                   help='Generate app_version.h C header  [default: on]')
    p.add_argument('--no-gen-header', action='store_false', dest='gen_header',
                   help='Do not generate app_version.h')

    return p.parse_args()


if __name__ == '__main__':
    try:
        sign_hex(parse_args())
    except Exception as e:
        print(f"\n  [ERROR] {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)