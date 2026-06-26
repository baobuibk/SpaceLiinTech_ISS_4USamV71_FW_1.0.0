# LSM6DSOX Test

## Expected coverage

* **LSM6DS0X Self Test** — Verify LSM6DSOX IMU is alive by reading WHO_AM_I register. Should return `Check OK` if the device responds with the expected ID (0x6C).

## Core LSM6D-test commands

| Command            | Arguments     | Expected Response                                                  |
| ------------------ | ------------- | ------------------------------------------------------------------ |
| `lsm6d_init`       | none          | Initializes the LSM6DSOX                                           |
| `lsm6d_conf_accel` | odr_xl fs_xl  | Configures Accelerometer ODR and Full Scale                        |
| `lsm6d_conf_gyro`  | odr_gy fs_gy  | Configures Gyroscope ODR and Full Scale                            |
| `lsm6d_conf_int`   | mode event    | Configures Interrupt mode and event                                |
| `lsm6d_conf_ff`    | ths_ff dur_ff | Sets Free-fall threshold and duration parameters                   |
| `lsm6d_conf_wu`    | ths_wu dur_wu | Sets Wake-up threshold and duration parameters                     |
| `lsm6d_dis_int`    | none          | Disables all active interrupts                                     |
| `lsm6d_check`      | none          | Verify LSM6DSOX is alive or not                                    |
| `lsm6d_read_accel` | none          | Reads and outputs 3-axis Accelerometer data (X, Y, Z) in mg        |
| `lsm6d_read_gyro`  | none          | Reads and outputs 3-axis Gyroscope data (X, Y, Z) in dps           |
| `lsm6d_read_temp`  | none          | Reads and outputs the internal sensor temperature in Celsius       |
| `lsm6d_read_all`   | none          | Reads and prints Accel (mg), Gyro (dps), and Temperature (°C) data |

## Command Usage

### 1. Initialize the LSM6DSOX sensor

**Command**

```text
lsm6dsox_init
```

**Expected Response**

```text
Configure Accelerometer: 208Hz, 8g
Configure Gyroscope: 208Hz, 1000dps
Init Success
```

### 2. Configure Accelerometer parameters

**Command**

```text
lsm6d_conf_accel <fre_id> <fullscale_id>
```

**Expected Response**

```text
Configure Accelerometer: 208Hz, 8g
```

**Arguments**

| fre_id | Description | fre_id | Description |
| ------ | ----------- | ------ | ----------- |
| `0`    | Power-down  | `4`    | 104 Hz      |
| `1`    | 12.5 Hz     | `5`    | 208 Hz      |
| `2`    | 26 Hz       | `6`    | 416 Hz      |
| `3`    | 52 Hz       | `7`    | 833 Hz      |
|        |             |        |             |

| fullscale_id | Description |
| ------------ | ----------- |
| `0`          | 2g          |
| `1`          | 16g         |
| `2`          | 4g          |
| `3`          | 8g          |

### 3. Configure Gyroscope parameters

**Command**

```text
lsm6d_conf_gyro <fre_id> <fullscale_id>
```

**Expected Response**

```text
Configure Gyroscope: 208Hz, 1000dps
```

**Arguments**

| fre_id | Description | fre_id | Description |
| ------ | ----------- | ------ | ----------- |
| `0`    | Power-down  | `4`    | 104 Hz      |
| `1`    | 12.5 Hz     | `5`    | 208 Hz      |
| `2`    | 26 Hz       | `6`    | 416 Hz      |
| `3`    | 52 Hz       | `7`    | 833 Hz      |
|        |             |        |             |

| fullscale_id | Description |
| ------------ | ----------- |
| `0`          | 125dps      |
| `1`          | 259dps      |
| `2`          | 500dps      |
| `3`          | 1000dps     |
| `4`          | 2000dps     |

### 4. Configure Gyroscope parameters

### 5. Configure Gyroscope parameters

### 6. Configure Gyroscope parameters

### 7. Configure Gyroscope parameters

### 8. Configure Gyroscope parameters

### 9. Configure Gyroscope parameters

### 10. Configure Gyroscope parameters

### 11. Configure Gyroscope parameters

### 12. Read all values from the LSM6DSOX

**Command**

```text
lsm6dsox_read_all
```

**Expected Response**

```text
Gyro  : X=0.07 dps   Y=-1.61 dps    Z=0.25 dps
Accel : X=-981.12 mg   Y=15.37 mg   Z=254.25 mg
Temp : 33.59 C
```

## Recommended Execution Flow

### 1. Base run

Step 1: Configure Accelerometer

```text
lsm6d_init
```

Step 2: Configure Gyroscope

```text
lsm6d_read_all
```

### 2. Event Setup Phase (Optional)

Option 1: Event Configuration

```text
lsm6d_conf_wu 3 0
```

or free-fall event

```text
lsm6d_conf_ff 3 0
```

Option 2: Configure Interrupts

```text
lsm6d_conf_int 0 6
```
