# Experiment Control Tutorial

## Purpose

This document describes the steps to configure and run a DLS (laser) experiment and how to validate data stored in PSRAM.


## Main commands and examples

| Command | Arguments | Expected Response |
|---|---|---|
| `dls_setup` | channel, laser_insentity, pre_time, main_time, post_time, photo_sample_rate, laser_sample_rate | Profile get parameter from console |
| `dls_start` | none | Validation all profile with togetther |

- **Step B1 — Setup the experiment:**

  `dls_setup <channel> <laser_intensity> <pre_ms> <sampling_ms> <post_ms> <photo_rate_ksps> <laser_rate_ksps>`

  Example:

  `dls_setup 24 100 1000 2000 1000 500 100`

- **Step B2 — Start the experiment:**

  `dls_start`

- **Step B3 — Validate PSRAM data (read):**

  `psram_read <psram_addr> <length>`

  Example:

  `psram_read 0x00 100`

- **Step B4 — Hand over PSRAM access to the MPU (manual):**

  `psram_sw_mpu`

- **Step B5 — Read PSRAM from the MPU:**

  The procedure for reading PSRAM on the MPU will be added later.

## Experiment sequence (summary)

1. Run the setup command (B1) with appropriate parameters.
2. Call `dls_start` (B2) to begin data acquisition.
3. Use `psram_read` (B3) to inspect data written to PSRAM.
4. If needed, transfer PSRAM access to the MPU by calling `psram_sw_mpu` (B4).
5. On the MPU, perform PSRAM read operations according to the MPU-specific procedure (to be documented).

## Notes

- PSRAM addresses are typically provided in hex (e.g., `0x00`).
- `length` is the number of bytes to read starting from the given address.
- `photo_rate_ksps` and `laser_rate_ksps` are sample rates in ksps (kilosamples per second).

If you want, I can add the MPU PSRAM read instructions once you provide the procedure or sample code.