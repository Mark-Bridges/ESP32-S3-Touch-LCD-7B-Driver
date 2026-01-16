// LovyanGFX Waveshare 7B Test - Working Version 🪂
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "gfx_adapter.h"
#include <driver/i2c.h>
// Vendor helpers
#include "gt911.h"        
#include "io_extension.h" 
#include "touch.h"  


void checkMemory(const char* location) {
    Serial0.printf("[MEMORY] %s - Free heap: %d bytes\n", location, ESP.getFreeHeap());
}
//
//
//
//
//
//
void setup() {
    Serial.begin(115200);delay(100);
    Serial0.begin(115200);delay(100);
    
    Serial0.println("\n\n=== LOVYANGFX WAVESHARE 7B TEST ===");
    checkMemory("Start");

// Basic hardware init
    touch_gt911_init();
    checkMemory("After Touch init");
    IO_EXTENSION_Init();
    checkMemory("After IO Ext init");
    // LCD
    // Initialize display
    // 2) Now bring up the RGB engine
  gfx->init();
  checkMemory("After GFX init");
  gfx->setRotation(0);

// Solid RGB blocks to verify channel order
gfx->fillScreen(0xF800); // RED should be red
delay(1000);
gfx->fillScreen(0x07E0); // GREEN should be green
delay(1000);
gfx->fillScreen(0x001F); // BLUE should be blue
delay(1000);

// draw RGB bars
gfx_probe_channel_bits();
delay(1000);
// Then draw bars again
gfx_test_pattern();
delay(1000);

// And draw ASCII text (no UTF-8, no fancy quotes)
gfx->setTextColor(0xFFFFFF, 0x000000); // opaque: white on black
gfx->drawString("ABC 123 Hello Lovyan!", 5, 48);
gfx->flush();;

gfx_test_full_screen();
  
checkMemory("End setup");
Serial.printf("[SETUP] Ready to test %d fonts\n", gfx_get_test_font_count());
}
//
//
//
//
//
//
void loop() {}

/*
void loop() {
    static uint32_t lastUpdate = 0;
    uint32_t ms = millis();
    
    if (ms - lastUpdate > FONT_CHANGE_INTERVAL) {
        lastUpdate = ms;
        
        currentFontTest++;
        if (currentFontTest >= gfx_get_test_font_count()) {
            currentFontTest = 0;
        }
        
        Serial.printf("[FONT] Testing %s\n", gfx_get_test_font_name(currentFontTest));
        checkMemory("Font change");
        
        testCurrentFont();
    }
    
    delay(100);
}*/