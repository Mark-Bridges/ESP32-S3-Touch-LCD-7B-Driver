#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

extern lgfx::v1::LGFX_Device* gfx;

// init & basic
void gfx_init();
void gfx_fill(uint32_t colour);
void gfx_flush();
void gfx_draw565(int x, int y, uint16_t* buf, int w, int h);
void gfx_test_pattern();
void gfx_probe_channel_bits();
void gfx_test_full_screen();

// text
void gfx_text(int x, int y, uint32_t colour, const char *msg);


// simple test-font stubs (to satisfy your .ino demo)
int         gfx_get_test_font_count();
const char* gfx_get_test_font_name(int idx);
void        gfx_set_test_font(int idx);


