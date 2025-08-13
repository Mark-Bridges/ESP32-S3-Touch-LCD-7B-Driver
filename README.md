# ESP32-S3-Touch-LCD-7B-Driver
A prototype driver for my new Waveshare 1024×600 7inch esp32 S3 display

# Waveshare ESP32‑S3 Touch LCD 7B – Driver Pack (v0.1)

**Goal:** a small, reusable layer that solves the two pain points:
- The hidden **I²C expander** that controls backlight / DISP / power.
- A simple **GT911 touch** wrapper with correct geometry.

This pack keeps display init *separate* so you can use either:
- Waveshare’s working `rgb_lcd_port.cpp` (ESP-IDF `esp_lcd` path), or
- Arduino_GFX (if you want, using the constants here).

## What’s inside

- `src/Waveshare7B_Constants.h` — 1024×600 timings & polarities.
- `src/WS7B_Expander.{h,cpp}` — minimal I²C expander (“EXIO”) driver.
- `src/WS7B_Touch_GT911.h` — tiny wrapper for TAMC_GT911.

## Basic usage (turn backlight on)

```cpp
#include <WS7B_Expander.h>
#include <Waveshare7B_Constants.h>

WS7B_Expander exio;

void setup() {
  Serial.begin(115200);
  // Waveshare 7B routes I²C on the touch pins: SDA=8, SCL=9
  if (!exio.begin(/*sda=*/8, /*scl=*/9)) {
    Serial.println("EXIO not found, continuing anyway...");
  }
  exio.forceAllOn();   // enables VDD_EN, DISP, backlight = 100%, SD_CS high
  Serial.println("EXIO forced ON.");
}

void loop(){}
```

## Touch

```cpp
#include <WS7B_Touch_GT911.h>
WS7B_Touch_GT911 touch;

void setup() {
  touch.setIntPin(4); // TP_INT
  touch.begin(8, 9);  // SDA=8, SCL=9
}

void loop() {
  uint16_t x,y;
  if (touch.read(x,y)) {
    Serial.printf("Touch: %u,%u\n", x, y);
  }
}
```

## Display init

- **Recommended (matches Waveshare demo):** include their `rgb_lcd_port.h/.cpp`
  and call `waveshare_esp32_s3_rgb_lcd_init();`. Use `gui_paint` to draw and
  `wavesahre_rgb_lcd_display()` to flush. This avoids Arduino_GFX signature drift.

- **Arduino_GFX:** use constants from `Waveshare7B_Constants.h` to build the
  `Arduino_ESP32RGBPanel(...)` with the short signature your library expects.
  Set `WS7B_PCLK_ACTIVE_NEG` and both HSYNC/VSYNC polarities to **0** by default.

## Notes

- This expander driver is conservative: it sets *all* pins to outputs and PWM 100%.
  Once your screen lights, we can map exact bits for BL, DISP, VDD_EN, SD_CS and
  expose a `setBacklight(uint8_t)` API.
- I²C addresses tried: `0x58`, then `0x5B`.
- Requires Arduino `Wire` and `TAMC_GT911` (if using the touch wrapper).

