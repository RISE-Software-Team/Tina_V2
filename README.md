# RISE – TINA v2 Avionics

Avionics system for the TINA_V2 rocket. Handles telemetry logging, flight event detection, and parachute deployment.

> **Note:** Main Flight Code runs on the Wio-E5-LE Mini, while the Nucleo-F042K6 acts as an interface for the pyro channels.

---

## Hardware

| Component | Part | Interface |
|-----------|------|-----------|
| Microcontroller | STM32F042K6 (Nucleo-F042K6) | — |
| IMU | BNO055 (9-axis) | I2C @ 0x28 |
| Barometer | BME280 | I2C @ 0x76 |
| Radio | Wio-E5-LE Mini (LoRa 868/915 MHz) | I2C |

4 pyro channels are available on the RFC board through Nucleo-F042K6 for drogue and main chute deployment.

---

## Flight States

```
PreFlight → PoweredAscent → DrogueDescent → MainDescent → Recovery
```

| State | Entry Condition | Key Actions |
|-------|----------------|-------------|
| **PreFlight** | Power on / arm switch | Sensor checks, arm pyros, verify LoRa link |
| **PoweredAscent** | Accel > 3g OR altitude > 10m | High-rate logging + telemetry |
| **DrogueDescent** | Apogee detected OR timeout | Fire drogue, continue logging |
| **MainDescent** | Drogue deployed + descending | Fire main at configured altitude (default 100m) |
| **Recovery** | Touchdown detected | Low-rate telemetry, stop pyros, keep logging |

Safety timeouts are in place: if apogee isn't detected within 20s of launch, drogue fires automatically.

---

## Telemetry

Radio packets use raw LoRa (not LoRaWAN). Two Wio-E5 modules are used, one on the sensor board, one connected to a laptop at the ground station.

Each packet has a fixed header + payload. Sensor values are sent as scaled integers (×100) to avoid floats.

**Telemetry packet fields:** ACC XYZ, GYRO XYZ, altitude, event flags, system state — **28 bytes total.**

**Event flags** (bitfield): `Bit0=Launch | Bit1=Drogue | Bit2=Main | Bit3=Touchdown`

**Error codes:** `IMU_FAIL (0x01)`, `BARO_FAIL (0x02)`, `LORA_FAIL (0x04)`, `PYRO_FAIL (0x05)`, and a few others.

---

## Key Config Parameters

Defined in a global config file (no SD card on this board).

| Parameter | Default | Description |
|-----------|---------|-------------|
| `LAUNCH_ACCEL_THRESHOLD_MS2` | 30 m/s² | Minimum accel to detect launch |
| `LAUNCH_ALTITUDE_THRESHOLD_M` | 5 m | Alt-based launch trigger fallback |
| `LAUNCH_TO_APOGEE_TIMEOUT_MS` | 20000 ms | Max time before forced drogue fire |
| `MAIN_DEPLOY_ALTITUDE` | 100 m | Altitude to deploy main chute |
| `TELEMETRY_INTERVAL_MS` | 30 ms | Radio transmit rate |

---

## Software Structure

```
core/
  main           – main loop, runs FSM
  flight_fsm     – manages all flight states
  pyro_manager   – pyro channel control
  telemetry      – packet building + transmission
utils/
  config         – loads all thresholds/params
  logger         – logging over radio
sensors/
  sensor         – IMU + barometer interface
```

---

## Pin Mapping (RFC – Nucleo-F042K6)

```
I2C_SDA  → PB7      I2C_SCL  → PB6
EN_PYRO1 → PA3      EN_PYRO2 → PA4
EN_PYRO3 → PA5      EN_PYRO4 → PA6
Pyro_Read_1 → PB4   Pyro_Read_2 → PB5
Pyro_Read_3 → PA8   Pyro_Read_4 → PB1
```

> `Pyro_Read` is **LOW** when fired or connection is broken. **HIGH** when off.

---
