# Temperature Control Test

## Expected coverage

- Configure a temperature profile
- Display and validate stored temperature profiles
- Configure PID parameters
- Enable automatic temperature control
- Start and stop temperature control
- Switch between automatic and manual modes
- Toggle temperature logging

## Core Temperature Control commands

| Command | Arguments | Expected Response |
|---|---|---|
| `temp_profile_cfg` | profile_index | Starts temperature profile configuration |
| `temp_profile_cfg_main_ntc` | ntc_index | Configures primary NTC sensor |
| `temp_profile_cfg_sec_ntc` | ntc_index | Configures secondary NTC sensor |
| `temp_profile_cfg_tec` | tec1 tec2 tec3 tec4 | Configures TEC |
| `temp_profile_cfg_heater` | heater1 ... heater8 | Configures Heater |
| `temp_profile_cfg_setpoint` | temperature(m°C) | Configures profile setpoint |
| `temp_profile_cfg_main_sec_delta` | delta(m°C) | Configures maximum temperature difference |
| `temp_profile_cfg_step` | step start_temp stop_temp duration | Adds or updates a profile step |
| `temp_profile_cfg_save` | none | Saves the configured profile |
| `temp_profile_cfg_cancel` | none | Cancels profile configuration |
| `temp_profile_dis` | profile_index | Displays the selected temperature profile |
| `temp_profile_val` | none | Validates all stored temperature profiles |
| `temp_auto_ena` | profile_index | Enables automatic temperature control mode |
| `temp_auto_start` | profile_index | Starts automatic temperature control |
| `temp_auto_pid_set` | state kp ki kd | Configures PID parameters |
| `temp_auto_pid_get` | state | Displays PID parameters |
| `temp_manu` | none | Switches to manual temperature control mode |
| `c` | none | Toggles temperature logging |

## Command Usage

### 1. Start profile configuration

**Command**

```text
temp_profile_cfg <profile_index>
```

**Arguments**

| profile_index | Description | profile_index | Description |
| ------------- | ----------- | ------------- | ----------- |
| `0`           | Profile 0   | `4`           | Profile 4   |
| `1`           | Profile 1   | `5`           | Profile 5   |
| `2`           | Profile 2   | `6`           | Profile 6   |
| `3`           | Profile 3   | `7`           | Profile 7   |

### 2. Configure main NTC

**Command**

```text
temp_profile_cfg_main_ntc <ntc_index>
```

**Arguments**

| ntc_index | Description | ntc_index | Description |
| ---------- | ----------- | --------- | ----------- |
| `0` | NTC_1 | `4` | NTC_5 |
| `1` | NTC_2 | `5` | NTC_6 |
| `2` | NTC_3 | `6` | NTC_7 |
| `3` | NTC_4 | `7` | NTC_8 |

### 3. Configure secondary NTC

**Command**

```text
temp_profile_cfg_sec_ntc <ntc_index>
```

**Arguments**

| ntc_index | Description | ntc_index | Description |
| ---------- | ----------- | --------- | ----------- |
| `0` | NTC_1 | `4` | NTC_5 |
| `1` | NTC_2 | `5` | NTC_6 |
| `2` | NTC_3 | `6` | NTC_7 |
| `3` | NTC_4 | `7` | NTC_8 |

### 4. Configure TEC use

**Command**

```text
temp_profile_cfg_tec <TEC_1> <TEC_2> <TEC_3> <TEC_4>
```

**Arguments**

| TEC_X | Description |
| ------ | ----------- |
| `0` | UNUSED |
| `1` | USED |

### 5. Configure Heater use

**Command**

```text
temp_profile_cfg_heater <HEATER_1> <HEATER_2> <HEATER_3> <HEATER_4> <HEATER_5> <HEATER_6> <HEATER_7> <HEATER_8>
```

**Arguments**

| HEATER_X | Description |
| -------- | ----------- |
| `0` | UNUSED |
| `1` | USED |

### 6. Configure temperature setpoint

**Command**

```text
temp_profile_cfg_setpoint <setpoint>
```

**Note**

```text
<setpoint> unit: milli-degree Celsius (m°C)
```

**Example**

```text
To configure a temperature setpoint of 25°C, execute:
temp_profile_cfg_setpoint 25000
```

### 7. Configure the maximum temperature difference between the primary and secondary NTC

**Command**

```text
temp_profile_cfg_main_sec_delta <delta_temperature>
```

**Note**

```text
<delta_temperature> unit: milli-degree Celsius (m°C)
```

**Example**

```text
To configure a temperature difference of 1°C, execute:
temp_profile_cfg_main_sec_delta 1000
```

### 8. Add profile step

**Command**

```text
temp_profile_cfg_step <step_index> <start_temp> <stop_temp> <duration>
```

**Arguments**

| Argument | Description |
|----------|-------------|
| `step_index` | Step index (0-9). |
| `start_temp` | Start temperature (m°C). |
| `stop_temp` | Target temperature (m°C). |
| `duration` | Running duration in seconds. |

**Note**

```text
To keep this step running indefinitely, set <duration> to 0.
```

### 9. Save profile

**Command**

```text
temp_profile_cfg_save
```

**Expected Response**

```text
Profile[profile_index] save successful
```

### 10. Cancel profile configuration

**Purpose**

```text
Cancel the current temperature profile configuration process.
```

**Command**

```text
temp_profile_cfg_cancel
```

**Expected Response**

```text
Temperature profile configuration cancelled
```

### 11. Display a profile

**Command**

```text
temp_profile_dis <profile_index>
```

**Arguments**

| profile_index | Description | profile_index | Description |
| ------------- | ----------- | ------------- | ----------- |
| `0` | Profile 0 | `4` | Profile 4 |
| `1` | Profile 1 | `5` | Profile 5 |
| `2` | Profile 2 | `6` | Profile 6 |
| `3` | Profile 3 | `7` | Profile 7 |

**Expected Response**

```text
====== Profile 0 ======
Enabled        : 0
Start          : 0
Main NTC       : 0
Sec NTC        : 1
TEC mask       : 0x01 (LSB: TEC1)
Heater mask    : 0x03 (LSB: HEATER1)
Setpoint       : 32.00 °C
Main-Sec Delta : 1.00 °C
Step Count     : 4
Step[0]: start=32.00°C, set=40.00°C, dur=60s
Step[1]: start=40.00°C, set=40.00°C, dur=50s
Step[2]: start=40.00°C, set=30.00°C, dur=120s
Step[3]: start=30.00°C, set=30.00°C, dur=0s
========================
```

### 12. Validate all profiles

**Purpose**

```text
Checks for resource conflicts between TECs and Heaters used by different profiles.

Run this command after configuring the profiles and before starting temperature control.
```

**Command**

```text
temp_profile_val
```

**Expected Response**

```text
[OK] All profiles are valid.
```

### 13. Enable temperature control

**Command**

```text
temp_auto_ena <profile_index>
```

**Arguments**

| profile_index | Description | profile_index | Description |
| ------------- | ----------- | ------------- | ----------- |
| `0` | Profile 0 | `4` | Profile 4 |
| `1` | Profile 1 | `5` | Profile 5 |
| `2` | Profile 2 | `6` | Profile 6 |
| `3` | Profile 3 | `7` | Profile 7 |

**Status**

```text
After executing this command, the system regulates the temperature to the configured setpoint and waits for the command to start the profile steps.

During this stage, temperature data is continuously printed to the console. Logging can be enabled or disabled using the `c` command.
```

### 14. Start automatic control

**Purpose**

```text
Start temperature control according to the configured profile steps.
```

**Command**

```text
temp_auto_start <profile_index>
```

**Arguments**

| profile_index | Description | profile_index | Description |
| ------------- | ----------- | ------------- | ----------- |
| `0` | Profile 0 | `4` | Profile 4 |
| `1` | Profile 1 | `5` | Profile 5 |
| `2` | Profile 2 | `6` | Profile 6 |
| `3` | Profile 3 | `7` | Profile 7 |

**Status**

```text
The system executes each profile step sequentially using the configured target temperatures and durations.
```

### 15. Configure PID parameters

**Command**

```text
temp_auto_pid_set 3 2.50 0.05 0.10
```

**Expected Response**

```text
PID parameters updated
```

### 16. Display PID parameters

**Command**

```text
temp_auto_pid_get 3
```

**Expected Response**

```text
State 3 PID:
Kp = 2.500
Ki = 0.050
Kd = 0.100
```

### 17. Switch to manual mode

**Command**

```text
temp_manu
```

**Expected Response**

```text
Manual temperature control enabled
```

### 18. Toggle temperature logging

**Command**

```text
c
```

**Expected Response**

```text
Temperature logging enabled
```

or

```text
Temperature logging disabled
```

## Recommended Execution Flow

### 1. Configure a new profile

```text
temp_profile_cfg 0
temp_profile_cfg_main_ntc 0
temp_profile_cfg_sec_ntc 1
temp_profile_cfg_tec 1 0 0 0
temp_profile_cfg_heater 1 0 0 0 0 0 0 0
temp_profile_cfg_setpoint 25000
temp_profile_cfg_main_sec_delta 1000
temp_profile_cfg_step 0 25000 30000 120
temp_profile_cfg_step 1 30000 30000 0
temp_profile_cfg_save
```

### 2. Validate the profile

```text
temp_profile_dis 0
temp_profile_val
```

### 3. Regulate the temperature to the configured setpoint

```text
temp_auto_ena 0
```

### 4. Monitor the temperature and disable logging

```text
c
```

### 5. Start profile execution

```text
temp_auto_start 0
```

### 6. Monitor the temperature and disable logging

```text
c
```