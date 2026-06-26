# Solenoid Control Tutorial

## Purpose

This document describes how to initialize and control the solenoids using the MAX22200 driver through the CLI.

---

## Main Commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `sln_hb_init` | None | Initialize all MAX22200 devices and configure all output channels. |
| `sln_ctl` | `<solenoid_index> <direction> <pulse_time_ms>` | Drive the selected solenoid in the specified direction for the specified pulse duration. |

---

## Command Usage

### 1. Initialize the Solenoid Driver

Initialize all MAX22200 devices before controlling any solenoid.

**Command**

```text
sln_hb_init
```

**Expected Response**

```text
OK: Solenoid Driver Initialized Successfully.
```

**Unexpected Response**

```text
ERROR: Solenoid Driver Init Failed.
```

---

### 2. Control a Solenoid

Drive a solenoid for a specified pulse duration.

**Command**

```text
sln_ctl <solenoid_index> <direction> <pulse_time_ms>
```

**Arguments**

| Argument | Description |
|----------|-------------|
| `solenoid_index` | Solenoid index (1–20). |
| `direction` | Driving direction (`0` or `1`). |
| `pulse_time_ms` | Pulse duration in milliseconds. |


**Examples**

Drive solenoid 1 in direction 1 for 30 ms:

```text
sln_ctl 1 1 30
```

Drive solenoid 1 in direction 0 for 30 ms:

```text
sln_ctl 1 0 30
```

**Expected Response**

```text
OK: Solenoid 1, Direction 1, Pulse 30 ms
```

**Unexpected Response**

```text
ERROR
```

---

## Typical Operation Sequence

- **Step B1 — Initialize all MAX22200 devices:**

  `sln_hb_init`

- **Step B2 — Drive the desired solenoid:**

  `sln_ctl 1 1 30`

---

## Notes

- `sln_hb_init` must be executed once after power-up (Current: Auto power-up).
- `solenoid_index` currently supports values from **1** to **20**.
- `direction` accepts only **0** or **1**.
- `pulse_time_ms` specifies how long the selected output remains active before being turned off automatically.
- The command is blocking and returns after the pulse duration has elapsed.
- If an invalid parameter is provided or a driver operation fails, an error message is returned.