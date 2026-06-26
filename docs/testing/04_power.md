# Power Control Test

## Expected coverage

- Turn ON/OFF all power rails
- Control power for individual modules
- Monitor power status
- Ensure correct power sequence

## Core Power-test commands

| Command | Arguments | Expected Response |
|---|---|---|
| `power_all_on` | none | Turns ON all power rails |
| `power_all_off` | none | Turns OFF all power rails |
| `power_all_get` | none | Displays status of all power rails |
| `power_som_on` | none | Turns ON SOM power |
| `power_som_off` | none | Turns OFF SOM power |
| `power_som_get` | none | Displays SOM power status |
| `power_buck_peri_on` | none | Turns ON peripheral buck power |
| `power_buck_peri_off` | none | Turns OFF peripheral buck power |
| `power_buck_peri_get` | none | Displays peripheral buck power status |
| `power_tec_on` | none | Turns ON TEC power |
| `power_tec_off` | none | Turns OFF TEC power |
| `power_tec_get` | none | Displays TEC power status |
| `power_hd4_on` | none | Turns ON HD4 power |
| `power_hd4_off` | none | Turns OFF HD4 power |
| `power_hd4_get` | none | Displays HD4 power status |
| `power_solenoid_on` | none | Turns ON solenoid power |
| `power_solenoid_off` | none | Turns OFF solenoid power |
| `power_solenoid_get` | none | Displays solenoid power status |
| `power_lp_on` | none | Turns ON LP module power |
| `power_lp_off` | none | Turns OFF LP module power |
| `power_lp_get` | none | Displays LP module power status |
| `power_heater_on` | none | Turns ON heater power |
| `power_heater_off` | none | Turns OFF heater power |
| `power_heater_get` | none | Displays heater power status |
| `power_ai_on` | none | Turns ON AI board power |
| `power_ai_off` | none | Turns OFF AI board power |
| `power_ai_get` | none | Displays AI board power status |

## Command Usage

### 1. Turn ON all power rails

**Command**

```text
power_all_on
```

**Expected Response**

```text
Power ON Success
```

### 2. Turn OFF all power rails

**Command**

```text
power_all_off
```

**Expected Response**

```text
Power OFF Success
```

### 3. Get all power rail status

**Command**

```text
power_all_get
```

**Expected Response**

```text
========== Power Tree ==========
SOM                : ON
Buck Peripherals   : ON
|-- TEC            : ON
|-- HD4            : ON
|-- Solenoid       : ON
LP                 : ON
Heater             : ON
AI Board           : OFF
================================
```

### 4. Control SOM power

**Command**

```text
power_som_on
```

or

```text
power_som_off
```

or

```text
power_som_get
```

**Expected Response**

```text
SOM Power: ON
```

### 5. Control Peripheral Buck power

**Command**

```text
power_buck_peri_on
```

or

```text
power_buck_peri_off
```

or

```text
power_buck_peri_get
```

**Expected Response**

```text
BUCK_PERI Power: ON
```

### 6. Control TEC power

**Command**

```text
power_tec_on
```

or

```text
power_tec_off
```

or

```text
power_tec_get
```

**Expected Response**

```text
TEC Power: ON
```

### 7. Control HD4 power

**Command**

```text
power_hd4_on
```

or

```text
power_hd4_off
```

or

```text
power_hd4_get
```

**Expected Response**

```text
HD4 Power: ON
```

### 8. Control Solenoid power

**Command**

```text
power_solenoid_on
```

or

```text
power_solenoid_off
```

or

```text
power_solenoid_get
```

**Expected Response**

```text
Solenoid Power: ON
```

### 9. Control LP module power

**Command**

```text
power_lp_on
```

or

```text
power_lp_off
```

or

```text
power_lp_get
```

**Expected Response**

```text
LP Power: ON
```

### 10. Control Heater power

**Command**

```text
power_heater_on
```

or

```text
power_heater_off
```

or

```text
power_heater_get
```

**Expected Response**

```text
Heater Power: ON
```

### 11. Control AI board power

**Command**

```text
power_ai_on
```

or

```text
power_ai_off
```

or

```text
power_ai_get
```

**Expected Response**

```text
AI Power: ON
```

## Recommended Execution Flow

### 1. Base run

Step 1: Turn ON all power rails

```text
power_all_on
```

Step 2: Verify all power rails

```text
power_all_get
```

Step 3: Turn OFF all power rails

```text
power_all_off
```

Step 4: Verify all power rails

```text
power_all_get
```

### 2. Individual Power Test (Optional)

Example: Test TEC power

```text
power_tec_on
power_tec_get
power_tec_off
power_tec_get
```

Example: Test SOM power

```text
power_som_on
power_som_get
power_som_off
power_som_get
```