# ESP32 Dual-LED PWM Fading with OLED Readout

This project controls two LEDs independently with ESP32 hardware PWM (LEDC), each at a different frequency, and shows each LED's live duty cycle and configured frequency on an SSD1306 I²C OLED.

- **Task 1:** Dual-LED fading with different PWM frequencies
- **Task 2:** Display PWM parameters on an OLED

> Items marked **[FILL IN]** must be completed with your own board details and measurements.

---

## 1. Hardware

| Item | Qty | Notes |
|---|---|---|
| ESP32 dev board | 1 | Chip variant: **[FILL IN: e.g. ESP32-WROOM-32 / ESP32-S3]** |
| LED (any colour) | 2 | |
| Resistor 220 Ω | 2 | One per LED, not shared |
| SSD1306 OLED, 128x64, I²C | 1 | Address usually `0x3C` |
| Breadboard + jumper wires | | |

### 1.1 Wiring

| Signal | ESP32 pin | Connects to |
|---|---|---|
| LED 1 | GPIO 18 | 220 Ω → LED1 anode; LED1 cathode → GND |
| LED 2 | GPIO 19 | 220 Ω → LED2 anode; LED2 cathode → GND |
| OLED SDA | GPIO 21 | OLED SDA |
| OLED SCL | GPIO 22 | OLED SCL |
| OLED VCC | 3V3 | OLED VCC |
| OLED GND | GND | OLED GND |

```
GPIO18 ──[220Ω]──►|── GND      LED 1  (1 kHz PWM)
GPIO19 ──[220Ω]──►|── GND      LED 2  (5 kHz PWM)

GPIO21 ─────────── SDA ┐
GPIO22 ─────────── SCL ├─ SSD1306 OLED (I²C, 0x3C)
3V3    ─────────── VCC ┤
GND    ─────────── GND ┘
```

**Resistor choice:** I ≈ (3.3 V − ~2 V LED drop) / 220 Ω ≈ 6 mA, safely within the ESP32 pin limit while still clearly visible. Each LED has its own resistor so the two channels cannot affect each other's current.

**Pin choice:** GPIO 18, 19, 21 and 22 are ordinary output-capable pins on the classic ESP32 dev board and are not boot-strapping pins. Avoid GPIO 6–11 (flash) and be careful with GPIO 0, 2, 12 and 15 (boot straps). If you use a different variant (S3, C3, etc.), re-check your pin map. **[FILL IN if pins differ]**

---

## 2. Software setup

- Framework: Arduino-ESP32, **core 3.x** (uses `ledcAttach(pin, freq, resolution)`)
- Libraries for Task 2: `Adafruit SSD1306` and `Adafruit GFX` (install via Library Manager)
- Board: select your ESP32 variant in the Arduino IDE; baud rate `115200`

Files:

| File | Purpose |
|---|---|
| `dual_led_pwm_fade.ino` | Task 1 sketch (fading + shared duty values) |
| **[FILL IN: Task 2 sketch name]** | Task 1 logic plus OLED output |

If you are on **core 2.x**, `ledcAttach()` does not exist. Use `ledcSetup(channel, freq, res)` + `ledcAttachPin(pin, channel)` and read section 5 before choosing channels.

---

## 3. Task 1: PWM configuration and duty-cycle control

### 3.1 PWM parameters

| | LED 1 | LED 2 |
|---|---|---|
| GPIO | 18 | 19 |
| PWM frequency | **1 kHz** (period 1 ms) | **5 kHz** (period 0.2 ms) |
| Duty resolution | 10 bit | 10 bit |
| Duty range | 0 – 1023 | 0 – 1023 |
| Fade step per update | 1 count | 2 counts |
| Fade update interval | 5 ms | 5 ms |

**Why 10 bits?** The LEDC hardware limit is `freq x 2^resolution <= timer clock` (80 MHz APB clock on the classic ESP32), so:

- 1 kHz: max resolution = log2(80,000,000 / 1,000) ≈ 16 bits
- 5 kHz: max resolution = log2(80,000,000 / 5,000) ≈ 13 bits

10 bits fits both frequencies, gives 1024 brightness levels (finer than 8-bit, which shows visible stepping at the dim end), and keeps the duty-to-percentage conversion simple (`duty / 1023`).

**Why these frequencies?** Both are well above the ~100 Hz flicker threshold of human vision, and the 5x ratio makes the difference easy to see on an oscilloscope or logic analyser.

### 3.2 How the fade works

- Each LED has a duty value that moves up by its step every 5 ms until it reaches 1023, then reverses and moves down to 0, repeating forever.
- The fade is **non-blocking** (`millis()` timing, no `delay()`), so the OLED code in Task 2 can run in the same `loop()` without stalling the fade.
- LED 2 steps twice as fast as LED 1, which makes it obvious that the two channels are independent.
- Optional gamma correction (`USE_GAMMA`, exponent 2.2) can be turned on so the fade *looks* linear to the eye. It is off by default so the raw duty ramp is linear and easy to verify.

### 3.3 Values shared with Task 2

| Variable | Meaning |
|---|---|
| `led1Duty`, `led2Duty` | Logical fade position, 0–1023 |
| `led1Applied`, `led2Applied` | The value actually passed to `ledcWrite()` (after optional gamma) |
| `getLed1DutyPercent()`, `getLed2DutyPercent()` | `applied / 1023 x 100` |

**The OLED must read the `...Applied` values.** They are the exact numbers written to the hardware, which satisfies the requirement that displayed values match the values actually used by the program.

---

## 4. Task 2: OLED output

### 4.1 Display layout (128x64)

```
LED1  1000 Hz
Duty: 45.2 %

LED2  5000 Hz
Duty: 67.9 %
```

| Field | Source |
|---|---|
| LED1 / LED2 frequency | The same constants passed to `ledcAttach()` (`LED1_FREQ_HZ`, `LED2_FREQ_HZ`) |
| LED1 / LED2 duty % | `getLed1DutyPercent()` / `getLed2DutyPercent()` |

### 4.2 Refresh behaviour

- The OLED is redrawn about every 100 ms (10 Hz), independent of the 5 ms fade update. A full 128x64 frame is 1024 bytes; at 400 kHz I²C this takes roughly 20-25 ms, so redrawing on every fade step would slow the fade. The 100 ms rate looks live to the eye and leaves the fade timing undisturbed.
- Set I²C to 400 kHz with `Wire.setClock(400000)`.
- The frequency line is static, so it shows the *configured* value. Duty percentages update continuously.
- The values are copied once per redraw so both lines on screen come from the same instant.

> **[FILL IN]** If your Task 2 code differs from the above (pins, address, refresh rate), edit this section to match what you actually implemented.

---

## 5. Technical check: LEDC timer / resource sharing

The task warns that not every ESP32 variant supports independent frequencies on any two pins. The relevant hardware facts are:

1. **LEDC structure.** Each LEDC *channel* drives one pin and is tied to one *timer*. The **timer** defines frequency and resolution. **Channels that share a timer must share the same frequency and resolution**; only duty can differ per channel.
2. **Any output-capable GPIO can be routed to any channel** through the GPIO matrix, so pin choice does not by itself limit frequencies. The limit is the number of timers.
3. **Resources per chip** (check the datasheet/TRM for your exact variant **[FILL IN]**):

   | Chip | Channels | Timers | Speed modes |
   |---|---|---|---|
   | ESP32 (classic) | 16 | 8 (4 per speed mode) | High-speed + low-speed |
   | ESP32-S2 / S3 | 8 | 4 | Low-speed only |
   | ESP32-C3 | 6 | 4 | Low-speed only |

   Two different frequencies therefore need **two different timers**, which every variant has.
4. **Arduino-ESP32 core 2.x.** `ledcSetup(channel, ...)` maps channels to timers in fixed pairs on the classic ESP32 (channels 0 and 1 share one timer, 2 and 3 share the next, and so on). Using channels 0 and 1 with different frequencies makes the second `ledcSetup()` overwrite the first, and **both LEDs end up at the same frequency**. Fix: use channels **0 and 2** (different timers).
5. **Arduino-ESP32 core 3.x.** `ledcAttach(pin, freq, res)` allocates the channel and a suitable timer automatically, giving each distinct frequency/resolution its own timer. This is the approach used here, so no manual channel management is needed. If the requested frequency/resolution cannot be achieved, `ledcAttach()` returns `false`, which the sketch should check.
6. **Resolution limit.** `freq x 2^resolution` must not exceed the timer clock (80 MHz APB assumed above). Other variants or clock sources can differ, so re-check the limit if you change chip or frequency.

**Limitation to document:** this design uses two of the available timers, leaving the others free. Adding more LEDs at *further* distinct frequencies would eventually exhaust the timers (4 per speed mode on most variants); LEDs sharing a frequency can share a timer.

**How to confirm independence in practice:** measure both pins with a scope or logic analyser (section 7). In core 3.x you can also print the configured values with `ledcReadFreq(pin)`. Both should report different values.

---

## 6. Theory

### 6.1 Duty cycle and apparent brightness

Duty cycle is the fraction of each PWM period in which the pin is HIGH:

`duty % = T_on / T_period x 100`

The LED is only ever fully on or fully off. Because it switches faster than the eye can follow, the eye averages the light, so average voltage ≈ `duty x 3.3 V` and perceived brightness rises with duty.

| Duty | Average voltage | Appearance |
|---|---|---|
| 0 % | 0 V | Off |
| 25 % | ≈ 0.83 V | Dim |
| 50 % | ≈ 1.65 V | Medium |
| 100 % | 3.3 V | Full brightness |

**Perception is not linear.** Human vision responds roughly logarithmically (a power law of about 2.2-2.8), so a linear duty ramp appears to brighten quickly at the start and barely change near the top. Gamma correction compensates for this when an *apparently* even fade is wanted.

### 6.2 PWM frequency and the output signal

Frequency sets the period and edge rate of the signal, not the average brightness. At equal duty, the 1 kHz and 5 kHz LEDs look equally bright.

| | 1 kHz | 5 kHz |
|---|---|---|
| Period | 1 ms | 0.2 ms |
| Edges per second | 2,000 | 10,000 |
| Max duty resolution (80 MHz clock) | ~16 bit | ~13 bit |
| Switching noise / EMI | Lower | Higher |

- Too low (below ~100 Hz): visible flicker, and banding on camera.
- Too high: less duty resolution available and more switching noise, with no visible benefit for an LED.
- For other loads frequency matters more: motors can whine at audible frequencies, and power converters depend on frequency for filter sizing.

---

## 7. Test procedure and results

### 7.1 Procedure

1. Wire the circuit as in section 1.1 and upload the sketch.
2. Open the Serial Monitor at 115200 baud and confirm duty values change.
3. Watch both LEDs: each should fade fully up and down smoothly, LED 2 faster than LED 1.
4. Probe GPIO 18 and GPIO 19 with an oscilloscope or logic analyser and measure frequency and duty.
5. Compare the OLED readout with the Serial output and the measured waveform at several moments.

### 7.2 Frequency check (**[FILL IN measured values]**)

| LED | Configured | Measured | Pass? |
|---|---|---|---|
| LED 1 (GPIO 18) | 1000 Hz | [FILL IN] | [ ] |
| LED 2 (GPIO 19) | 5000 Hz | [FILL IN] | [ ] |

### 7.3 Duty-cycle check (**[FILL IN measured values]**)

Expected % = `duty / 1023 x 100`.

| Sample | Applied duty | Expected % | OLED % | Measured % (scope) |
|---|---|---|---|---|
| LED 1, minimum | 0 | 0.0 | [FILL IN] | [FILL IN] |
| LED 1, mid | 512 | 50.0 | [FILL IN] | [FILL IN] |
| LED 1, maximum | 1023 | 100.0 | [FILL IN] | [FILL IN] |
| LED 2, minimum | 0 | 0.0 | [FILL IN] | [FILL IN] |
| LED 2, mid | 512 | 50.0 | [FILL IN] | [FILL IN] |
| LED 2, maximum | 1023 | 100.0 | [FILL IN] | [FILL IN] |

### 7.4 Functional checks

| Check | Result |
|---|---|
| Both LEDs fade 0 → max → 0 repeatedly | [FILL IN] |
| LEDs fade independently (different speeds) | [FILL IN] |
| Different frequencies confirmed on scope | [FILL IN] |
| OLED shows both frequencies correctly | [FILL IN] |
| OLED duty % tracks the fade live | [FILL IN] |
| OLED % matches Serial output | [FILL IN] |
| No visible flicker on either LED | [FILL IN] |

### 7.5 Observations

**[FILL IN]** Any notes, such as how the linear ramp looked versus gamma-corrected, any flicker seen on camera, or deviations from expected values.

---

## 8. Troubleshooting

| Symptom | Likely cause / fix |
|---|---|
| `ledcAttach was not declared` | You are on core 2.x. Use `ledcSetup` + `ledcAttachPin`, with channels 0 and 2 |
| Both LEDs show the same frequency | They share a LEDC timer (core 2.x channels 0/1). Use channels 0 and 2 |
| OLED blank | Check wiring, I²C address (`0x3C` vs `0x3D`), and that `display.begin()` returned true |
| Fade looks jerky | Something is blocking `loop()`. Remove `delay()` calls and keep OLED redraws at ~10 Hz |
| LED never fully off/on | Check the resistor and LED polarity (long leg = anode) |