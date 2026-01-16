#define LGFX_USE_V1
#include "gfx_adapter.h"

// v1.2.9 headers for RGB path
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp> // \lgfx\v1\platforms\esp32s3\Bus_RGB.hpp
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/Light.hpp>

class MyLGFX : public lgfx::v1::LGFX_Device {
  lgfx::v1::Bus_RGB    _bus;
  lgfx::v1::Panel_RGB  _panel;
  lgfx::v1::Light_PWM  _light;

public:
  MyLGFX() {
    // --- Bus config ---
    {
      auto b = _bus.config();
      b.panel = &_panel;

// ==== DATA PINS (BGR channels, bits reversed within each channel) ====
// B4..B0 on D0..D4
    b.pin_d0  = 10;  // B4 (MSB)
    b.pin_d1  = 17;  // B3
    b.pin_d2  = 38;  // B2
    b.pin_d3  = 14;  // B1
    b.pin_d4  = 15;  // B0 (LSB)

// G5..G0 on D5..D10
    b.pin_d5  = 21;  // G5 (MSB)
    b.pin_d6  = 47;  // G4
    b.pin_d7  = 48;  // G3
    b.pin_d8  = 45;  // G2
    b.pin_d9  = 0;   // G1
    b.pin_d10 = 39;  // G0 (LSB)

// R4..R0 on D11..D15
    b.pin_d11 = 40;  // R4 (MSB)
    b.pin_d12 = 41;  // R3
    b.pin_d13 = 42;  // R2
    b.pin_d14 = 2;   // R1
    b.pin_d15 = 1;   // R0 (LSB)


      b.pin_henable = 5; //DE     cfg.pin_henable
      b.pin_vsync = 3;
      b.pin_hsync = 46;
      b.pin_pclk  = 7;

b.freq_write        = 12000000;    // start low; we'll turn it up after text works

b.hsync_polarity    = 0;
b.hsync_front_porch = 40;
b.hsync_pulse_width = 48;
b.hsync_back_porch  = 40;

b.vsync_polarity    = 0;
b.vsync_front_porch = 4;
b.vsync_pulse_width = 20;
b.vsync_back_porch  = 4;


b.pclk_active_neg   = 0;          // test BOTH 0 and 1
b.de_idle_high      = 0;          // test BOTH 0 and 1 if needed

      _bus.config(b);
      _panel.setBus(&_bus);
      //b.output_bgr = true;   // swap R↔B channels in hardware output


    }

    // --- Panel config ---
    {
      auto p = _panel.config();
      p.memory_width  = 1024;
      p.memory_height = 600;
      p.panel_width   = 1024;
      p.panel_height  = 600;
      p.offset_x = 0;
      p.offset_y = 0;
      _panel.config(p);
    

      //auto pd = _panel.config_detail();
      //pd.use_psram = 1;   // if you have PSRAM; 0 if not
      //_panel.config_detail(pd);
    }

    // --- Backlight (optional; set your BL pin if used) ---
    { // --- Backlight (PWM on an MCU GPIO) ---
      auto l = _light.config();
      l.pin_bl      = 2;       // <-- your real BL pin here (GPIO)
      l.freq        = 20000;   // 20 kHz
      l.pwm_channel = 7;       // any free LEDC channel
      _light.config(l);
      _panel.setLight(&_light);
      _light.setBrightness(255); // 0–255 default at power-on
    }

    setPanel(&_panel);
  }
};

// single instance exported as base pointer
static MyLGFX s_gfx;
lgfx::v1::LGFX_Device* gfx = &s_gfx;

// ========= API =========
void gfx_init() {
  gfx->init();
  gfx->setColorDepth(16);                           // interpret 0xF800 etc. as RGB565
    gfx->setTextWrap(false);
    gfx->setTextDatum(lgfx::v1::textdatum_t::top_left);
    gfx->setFont(&fonts::Font2);                      // any bitmap font (0/2/4/6/7/8)
    gfx->setTextSize(1);
    gfx->fillScreen(0x0000);
  gfx->flush();
}

void gfx_fill(uint32_t c) { if (gfx) gfx->fillScreen(c); }
void gfx_flush()          { if (gfx) gfx->flush(); }
void gfx_draw565(int x, int y, uint16_t* buf, int w, int h) {
  if (gfx) gfx->pushImage(x, y, w, h, (lgfx::v1::rgb565_t*)buf);
}


void gfx_text(int x, int y, uint32_t colour, const char *msg) {
  if (!gfx || !msg) return;
  //if (size < 1) size = 1;
  gfx->setTextWrap(false);
  gfx->setTextDatum(lgfx::v1::textdatum_t::top_left);
  gfx->setTextColor(colour);
  gfx->drawString(msg, x, y);
}



// demo stubs (match your .ino)
int  gfx_get_test_font_count()        { return 1; }
const char* gfx_get_test_font_name(int){ return "Font0"; }
void gfx_set_test_font(int)           { if (gfx) gfx->setFont(&fonts::Font0); }

// Optional font switch
void gfx_font_classic() {
  if (!gfx) return;
  gfx->setFont(&fonts::Font0);  // built-in bitmap ASCII font
}

// Font testing
static const lgfx::IFont* TEST_FONTS[] = {
    &fonts::Font0,  // Default 8x8
    //&fonts::Font1,  // 9px
    &fonts::Font2,  // 12px
    &fonts::Font4,  // 16px  
    &fonts::Font6,  // 24px
    &fonts::Font8,  // 32px
};

static const char* TEST_FONT_NAMES[] = {
    "Font0 (8x8)",
    //"Font1 (9px)",
    "Font2 (12px)", 
    "Font4 (16px)",
    "Font6 (24px)",
    "Font8 (32px)",
};

static const int TEST_FONT_COUNT = sizeof(TEST_FONTS) / sizeof(TEST_FONTS[0]);

void gfx_test_pattern() {
// 24-bit literals; LGFX will downconvert to your setColorDepth
uint32_t ORANGE  = 0xFFA500;
uint32_t RED     = 0xFF0000;
uint32_t GREEN   = 0x00FF00;
uint32_t BLUE    = 0x0000FF;
uint32_t YELLOW  = 0xFFFF00;
uint32_t CYAN    = 0x00FFFF;
uint32_t MAGENTA = 0xFF00FF;
uint32_t WHITE   = 0xFFFFFF;
uint32_t BLACK   = 0x000000;
    
    static const uint32_t C[8] = {ORANGE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE};
    
    const int w = 1024 / 8;
    for(int i = 0; i < 8; ++i) { 
        int ww = (i == 7) ? (1024 - i * w) : w; 
        gfx->fillRect(i * w, 0, ww, 600, C[i]); 
    }
    
    gfx->setFont(&fonts::Font4);              // or Font0/4/6/8
gfx->setTextSize(1);                      // bitmap font scale
gfx->setTextColor(RED, BLACK);
gfx->setTextWrap(false);                  // <— important
gfx->setTextDatum(lgfx::v1::textdatum_t::top_left);  // <— important
gfx->drawString("LovyanGFX Test - Working!", 5, 100);
gfx->flush();

}

// Draw vertical bands for each bit of each channel.
// You should see 5 bands for RED (left), 6 for GREEN (middle), 5 for BLUE (right)
// with increasing brightness from LSB → MSB.
void gfx_probe_channel_bits() {
  gfx->fillScreen(0x0000);

  const int W = 1024, H = 600;

  // ---- RED bits (5) ----
  for (int i = 0; i < 5; ++i) {
    uint16_t c = ((1 << i) & 0x1F) << 11;           // red bit i only
    int x = i * (W / 16);
    gfx->fillRect(x, 0, (W/16)-2, H, c);
  }

  // ---- GREEN bits (6) ----
  for (int i = 0; i < 6; ++i) {
    uint16_t c = ((1 << i) & 0x3F) << 5;            // green bit i only
    int x = (5 + i) * (W / 16);
    gfx->fillRect(x, 0, (W/16)-2, H, c);
  }

  // ---- BLUE bits (5) ----
  for (int i = 0; i < 5; ++i) {
    uint16_t c = ((1 << i) & 0x1F);                 // blue bit i only
    int x = (11 + i) * (W / 16);
    gfx->fillRect(x, 0, (W/16)-2, H, c);
  }

  // Labels (opaque so we see them even if text is odd)
  gfx->setTextWrap(false);
  gfx->setFont(&fonts::Font2);
  gfx->setTextSize(1);
  gfx->setTextColor(0xFFFF, 0x0000);  // white on black (FG,BG)
  gfx->drawString("R0  R1  R2  R3  R4    G0  G1  G2  G3  G4  G5    B0  B1  B2  B3  B4", 12, 16);
  gfx->flush();
}

void gfx_test_full_screen() {
    if (!gfx) return;
    
    // Test full width
    gfx->fillScreen(0x000000);
    
    // Draw border around entire screen
    gfx->drawRect(0, 0, 1024, 600, 0xFFFFFF);
    
    // Draw text at various positions across the screen
    gfx->setTextColor(0xFFFFFF, 0x000000);
    gfx->setFont(&fonts::Font2);
    gfx->setTextSize(1);
    
    // Left side
    gfx->drawString("Left", 10, 10);
    // Center  
    gfx->drawString("Centre", 512, 300);
    // Right side
    gfx->drawString("Right", 950, 570);
    
    // Draw a line across the entire width
    gfx->drawLine(0, 290, 1022, 290, 0x00FF00);

    gfx->drawLine(0, 330, 1022, 330, 0x00FF00);
    gfx->flush();
}