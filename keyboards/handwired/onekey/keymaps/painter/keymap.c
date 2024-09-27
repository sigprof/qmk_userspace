// Copyright 2024 Sergey Vlasov <sigprof@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "color.h"
#include "qp.h"
#include "qp_comms.h"

enum tap_dances {
    TD_QP,
};

enum test_modes {
    TEST_FIRST,
    TEST_COLOR_GRID = TEST_FIRST,
    TEST_CHARACTERS_ON_BLACK,
    TEST_CHARACTERS_ON_WHITE,
    TEST_SLOW_UPDATE_ON_BLACK,
    TEST_SLOW_UPDATE_ON_WHITE,
    TEST_ALL_WHITE,
    TEST_WHITE_FRAME,
    TEST_BLACK_FRAME,
    TEST_ALL_BLACK,
    TEST_FILL_HORZ_0,
    TEST_FILL_HORZ_1,
    TEST_FILL_VERT_0,
    TEST_FILL_VERT_1,
    TEST_FILL_CHECKERBOARD_1,
    TEST_FILL_CHECKERBOARD_2,
    TEST_FILL_CHECKERBOARD_4,
    TEST_LAST = TEST_FILL_CHECKERBOARD_4,
};

enum font_sizes {
    FONT_FIRST,
    FONT_6x9 = FONT_FIRST,
    FONT_6x13,
    FONT_8x13,
    FONT_9x15,
    FONT_10x20,
    FONT_LAST = FONT_10x20,
};

static enum test_modes    test_mode = TEST_FIRST;
static enum font_sizes    font_size = FONT_FIRST;
static painter_rotation_t rotation  = QP_ROTATION_0;

static bool     need_update = true;
static bool     restart_test;
static bool     update_speed_test;
static uint32_t update_speed_start_timer;
static uint16_t update_speed_count;

static painter_device_t      screen;
static painter_font_handle_t font;

#if defined(QUANTUM_PAINTER_ST7735_SPI_ENABLE)

// The ST7735(S) 128*160 LCD from AliExpress apparently uses the RGB color
// order, not BGR, so the init routine needs to be redefined.

#    include "qp_st77xx_opcodes.h"
#    include "qp_st7735_opcodes.h"

bool qp_st7735_init(painter_device_t device, painter_rotation_t rotation) {
    // clang-format off
    const uint8_t st7735_init_sequence[] = {
        // Command,                 Delay, N, Data[N]
        ST77XX_CMD_RESET,            120,  0,
        ST77XX_CMD_SLEEP_OFF,          5,  0,
        ST77XX_SET_PIX_FMT,            0,  1, 0x55,
        ST77XX_CMD_INVERT_OFF,         0,  0,
        ST77XX_CMD_NORMAL_ON,          0,  0,
        ST7735_SET_PGAMMA,             0, 16, 0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,
        ST7735_SET_NGAMMA,             0, 16, 0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
        ST77XX_CMD_DISPLAY_ON,        20,  0
    };
    // clang-format on
    qp_comms_bulk_command_sequence(device, st7735_init_sequence, sizeof(st7735_init_sequence));

    // Configure the rotation (i.e. the ordering and direction of memory writes in GRAM)
    const uint8_t madctl[] = {
        [QP_ROTATION_0]   = ST77XX_MADCTL_RGB,
        [QP_ROTATION_90]  = ST77XX_MADCTL_RGB | ST77XX_MADCTL_MX | ST77XX_MADCTL_MV,
        [QP_ROTATION_180] = ST77XX_MADCTL_RGB | ST77XX_MADCTL_MX | ST77XX_MADCTL_MY,
        [QP_ROTATION_270] = ST77XX_MADCTL_RGB | ST77XX_MADCTL_MV | ST77XX_MADCTL_MY,
    };
    qp_comms_command_databyte(device, ST77XX_SET_MADCTL, madctl[rotation]);

    return true;
}

static painter_device_t make_screen(void) {
    return qp_st7735_make_spi_device(128, 160, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, 4, 0);
}

#elif defined(QUANTUM_PAINTER_SH1106_I2C_ENABLE)

#    if defined(SH1107_64x128)

#        include "qp_oled_panel.h"
#        include "qp_sh1106.h"
#        include "qp_sh1106_opcodes.h"

bool qp_sh1106_init(painter_device_t device, painter_rotation_t rotation) {
    oled_panel_painter_device_t *oled = (oled_panel_painter_device_t *)device;

    // Change the surface geometry based on the panel rotation
    if (rotation == QP_ROTATION_90 || rotation == QP_ROTATION_270) {
        oled->surface.base.panel_width  = oled->base.panel_height;
        oled->surface.base.panel_height = oled->base.panel_width;
    } else {
        oled->surface.base.panel_width  = oled->base.panel_width;
        oled->surface.base.panel_height = oled->base.panel_height;
    }

    // Init the internal surface
    if (!qp_init(&oled->surface.base, QP_ROTATION_0)) {
        qp_dprintf("Failed to init internal surface in qp_sh1106_init\n");
        return false;
    }

    // clang-format off
    uint8_t sh1106_init_sequence[] = {
        // Command,                 Delay,  N, Data[N]
        SH1106_SET_MUX_RATIO,           0,  1, 0x7F,
        SH1106_DISPLAY_OFFSET,          0,  1, 0,
        SH1106_DISPLAY_START_LINE,      0,  0,
        SH1106_SET_SEGMENT_REMAP_INV,   0,  0,
        SH1106_COM_SCAN_DIR_DEC,        0,  0,
        SH1106_COM_PADS_HW_CFG,         0,  1, 0x12,
        SH1106_SET_CONTRAST,            0,  1, 0x7F,
        SH1106_ALL_ON_RESUME,           0,  0,
        SH1106_NON_INVERTING_DISPLAY,   0,  0,
        SH1106_SET_OSC_DIVFREQ,         0,  1, 0x80,
        SH1106_SET_CHARGE_PUMP,         0,  1, 0x14,
        SH1106_DISPLAY_ON,              0,  0,
    };
    // clang-format on

    // If the display width is anything other than the default 128 pixels, change SH1106_SET_MUX_RATIO data byte to the correct value.
    // Note that this part is specific to SH1107 (unlike SH1106 and SSD1306, the mux ratio affect columns instead of rows).
    if (oled->base.panel_width != 128) {
        sh1106_init_sequence[3] = oled->base.panel_width - 1;
    }

    // If the display width is less than the default 128 pixels, change SH1106_DISPLAY_OFFSET to use the center columns.
    // This is again SH1107-specific.
    if (oled->base.panel_width < 128) {
        sh1106_init_sequence[7] = (128U - oled->base.panel_width) / 2;
    }

    // For 128x32 or 96x16 displays, change SH1106_COM_PADS_HW_CFG data byte from alternative (0x12) to sequential (0x02) configuration
    if (oled->base.panel_height <= 32) {
        sh1106_init_sequence[20] = 0x02;
    }

    qp_comms_bulk_command_sequence(device, sh1106_init_sequence, sizeof(sh1106_init_sequence));
    return true;
}

static painter_device_t make_screen(void) {
    return qp_sh1106_make_i2c_device(64, 128, 0x3C);
}

#    else

// Assume the usual 128x32 SSD1306 OLED.
static painter_device_t make_screen(void) {
    return qp_sh1106_make_i2c_device(128, 32, 0x3C);
}

#    endif

#elif defined(QUANTUM_PAINTER_ILI9341_SPI_ENABLE)

#    include "qp_ili9341.h"
#    include "qp_ili9xxx_opcodes.h"

// Configuration for the MSP2834 display module:
//   http://www.lcdwiki.com/2.8inch_IPS_SPI_Module_ILI9341
// (with the default config the image is inverted)
bool qp_ili9341_init(painter_device_t device, painter_rotation_t rotation) {
    // clang-format off
    const uint8_t ili9341_init_sequence[] = {
        // Command,                 Delay,  N, Data[N]
        ILI9XXX_CMD_RESET,            120,  0,
        ILI9XXX_CMD_SLEEP_OFF,          5,  0,
        ILI9XXX_POWER_CTL_A,            0,  5, 0x39, 0x2C, 0x00, 0x34, 0x02,
        ILI9XXX_POWER_CTL_B,            0,  3, 0x00, 0xD9, 0x30,
        ILI9XXX_POWER_ON_SEQ_CTL,       0,  4, 0x64, 0x03, 0x12, 0x81,
        ILI9XXX_SET_PUMP_RATIO_CTL,     0,  1, 0x20,
        ILI9XXX_SET_POWER_CTL_1,        0,  1, 0x26,
        ILI9XXX_SET_POWER_CTL_2,        0,  1, 0x11,
        ILI9XXX_SET_VCOM_CTL_1,         0,  2, 0x35, 0x3E,
        ILI9XXX_SET_VCOM_CTL_2,         0,  1, 0xBE,
        ILI9XXX_DRV_TIMING_CTL_A,       0,  3, 0x85, 0x10, 0x7A,
        ILI9XXX_DRV_TIMING_CTL_B,       0,  2, 0x00, 0x00,
        ILI9XXX_SET_BRIGHTNESS,         0,  1, 0xFF,
        ILI9XXX_ENABLE_3_GAMMA,         0,  1, 0x00,
        ILI9XXX_SET_GAMMA,              0,  1, 0x01,
        ILI9XXX_SET_PGAMMA,             0, 15, 0x0F, 0x29, 0x24, 0x0C, 0x0E, 0x09, 0x4E, 0x78, 0x3C, 0x09, 0x13, 0x05, 0x17, 0x11, 0x00,
        ILI9XXX_SET_NGAMMA,             0, 15, 0x00, 0x16, 0x1B, 0x04, 0x11, 0x07, 0x31, 0x33, 0x42, 0x05, 0x0C, 0x0A, 0x28, 0x2F, 0x0F,
        ILI9XXX_CMD_INVERT_ON,          0,  0,
        ILI9XXX_SET_PIX_FMT,            0,  1, 0x05,
        ILI9XXX_SET_FRAME_CTL_NORMAL,   0,  2, 0x00, 0x1B,
        ILI9XXX_SET_FUNCTION_CTL,       0,  2, 0x0A, 0xA2,
        ILI9XXX_CMD_PARTIAL_OFF,        0,  0,
        ILI9XXX_CMD_DISPLAY_ON,        20,  0
    };
    // clang-format on
    qp_comms_bulk_command_sequence(device, ili9341_init_sequence, sizeof(ili9341_init_sequence));

    // Configure the rotation (i.e. the ordering and direction of memory writes in GRAM)
    const uint8_t madctl[] = {
        [QP_ROTATION_0]   = ILI9XXX_MADCTL_BGR,
        [QP_ROTATION_90]  = ILI9XXX_MADCTL_BGR | ILI9XXX_MADCTL_MX | ILI9XXX_MADCTL_MV,
        [QP_ROTATION_180] = ILI9XXX_MADCTL_BGR | ILI9XXX_MADCTL_MX | ILI9XXX_MADCTL_MY,
        [QP_ROTATION_270] = ILI9XXX_MADCTL_BGR | ILI9XXX_MADCTL_MV | ILI9XXX_MADCTL_MY,
    };
    qp_comms_command_databyte(device, ILI9XXX_SET_MEM_ACS_CTL, madctl[rotation]);

    return true;
}

static painter_device_t make_screen(void) {
    return qp_ili9341_make_spi_device(240, 320, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, 4, 0);
}

#elif defined(QUANTUM_PAINTER_ST7567S_I2C_ENABLE)

static painter_device_t make_screen(void) {
    return qp_st7567s_make_i2c_device(128, 64, 0x3F);
}

#else
#    error "Display driver type not defined or unknown"
#endif

// Color grid: 8 columns of colors with 100% saturation and varying value, then
// 8 columns of the same colors with 50% saturation.
static void test_color_grid(void) {
    static const HSV colors[8] = {
        {HSV_BLACK}, {HSV_RED}, {HSV_GREEN}, {HSV_YELLOW}, {HSV_BLUE}, {HSV_MAGENTA}, {HSV_CYAN}, {HSV_WHITE},
    };
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_BLACK, true);
    uint16_t rw = screen_w / 16;
    uint16_t rh = screen_h / 16;
    for (int i = 0; i < 16; ++i) {
        HSV hsv = colors[i % 8];
        if (i >= 8) {
            hsv.s /= 2;
        }
        for (int j = 0; j < 16; ++j) {
            qp_rect(screen, i * rw, j * rh, (i + 1) * rw - 1, (j + 1) * rh - 1, hsv.h, hsv.s, hsv.v * j / 15, true);
        }
    }
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_WHITE, false);
}

// Use all 94 visible ASCII characters for testing.
#define TEST_CHAR_COUNT ('~' - '!' + 1)

static char get_test_char(uint8_t char_index) {
    return char_index + '!';
}

static void swap_hsv(HSV *a, HSV *b) {
    HSV tmp = *a;
    *a      = *b;
    *b      = tmp;
}

// Fill the whole screen with distinct characters (if the display is large enough to show more than 94 characters
// at once, the sequence is repeated again with inverted characters).
static void test_characters(bool invert) {
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    uint16_t char_w   = qp_textwidth(font, "0");
    uint16_t char_h   = font->line_height;
    uint16_t cols     = screen_w / char_w;
    uint16_t rows     = screen_h / char_h;

    uint8_t char_index = 0;

    HSV bg = {HSV_BLACK};
    HSV fg = {HSV_WHITE};
    if (invert) {
        swap_hsv(&bg, &fg);
    }

    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, bg.h, bg.s, bg.v, true);
    for (uint8_t row = 0; row < rows; ++row) {
        for (uint8_t col = 0; col < cols; ++col) {
            char str[2];
            str[0] = get_test_char(char_index);
            str[1] = '\0';
            qp_drawtext_recolor(screen, col * char_w, row * char_h, font, str, fg.h, fg.s, fg.v, bg.h, bg.s, bg.v);
            if (++char_index >= TEST_CHAR_COUNT) {
                char_index = 0;
                swap_hsv(&bg, &fg);
            }
        }
    }
}

// Test screen updating after drawing a single character or pixel.
void test_slow_update(bool invert) {
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    uint16_t char_w   = qp_textwidth(font, "0");
    uint16_t char_h   = font->line_height;
    uint16_t cols     = screen_w / char_w;
    uint16_t rows     = screen_h / char_h;

    HSV bg = {HSV_BLACK};
    HSV fg = {HSV_WHITE};
    if (invert) {
        swap_hsv(&bg, &fg);
    }

    static uint8_t  phase, char_index, first_char;
    static uint16_t x, y;
    static uint16_t timer;
    static uint16_t delay = 500;

    if (restart_test) {
        // Initialize all state variables before starting the test.
        restart_test = false;
        phase        = 0;
        x            = 0;
        y            = 0;
        char_index   = 0;
        first_char   = 0;
        delay        = 500;

        // Clear the whole screen at the start of test.
        qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, bg.h, bg.s, bg.v, true);
    } else {
        // Wait for the specified time between steps.
        if (timer_elapsed(timer) < delay) {
            return;
        }
    }

    timer = timer_read();
    switch (phase) {
        case 0:
            // Phase 0: fill the whole screen with mostly distinct characters, one character at a time.  Here the
            // inversion trick is not used, so that the frame which is drawn in subsequent phases would not be
            // overlapped by the inverted character background.
            char str[2];
            str[0] = get_test_char(char_index);
            str[1] = '\0';
            qp_drawtext_recolor(screen, x * char_w, y * char_h, font, str, fg.h, fg.s, fg.v, bg.h, bg.s, bg.v);
            if (++char_index >= TEST_CHAR_COUNT) {
                char_index = 0;
            }
            if (++x >= cols) {
                x = 0;
                if (++y >= rows) {
                    // The whole screen was filled - start the next phase.
                    ++phase;
                    x = y = 0;
                }
            }
            delay = 250;
            break;

        case 1:
            // Phase 1: draw a line along the left edge of the screen, one pixel at a time.
            qp_setpixel(screen, x, y, fg.h, fg.s, fg.v);
            if (y < screen_h - 1) {
                ++y;
            } else {
                // The bottom left corner is reached - start the next phase.
                ++phase;
                ++x;
            }
            delay = 50;
            break;

        case 2:
            // Phase 2: draw a line along the bottom edge of the screen, one pixel at a time.
            qp_setpixel(screen, x, y, fg.h, fg.s, fg.v);
            if (x < screen_w - 1) {
                ++x;
            } else {
                // The bottom right corner was reached - start the next phase.
                ++phase;
                --y;
            }
            delay = 50;
            break;

        case 3:
            // Phase 3: draw a line along the right edge of the screen, one pixel at a time.
            qp_setpixel(screen, x, y, fg.h, fg.s, fg.v);
            if (y > 0) {
                --y;
            } else {
                // The top right corner was reached - start the next phase.
                ++phase;
                --x;
            }
            delay = 50;
            break;

        case 4:
            // Phase 4: draw a line along the top edge of the screen, one pixel at a time.
            qp_setpixel(screen, x, y, fg.h, fg.s, fg.v);
            if (x > 0) {
                --x;
            } else {
                // The top left corner was reached - start the next phase.
                ++phase;
            }
            delay = 50;
            break;

        default:
            // Restart from phase 0, but change the first character of the sequence to make screen updates visible.
            if (++first_char >= TEST_CHAR_COUNT) {
                first_char = 0;
            }
            phase      = 0;
            x          = 0;
            y          = 0;
            char_index = first_char;
            delay      = 500;
            break;
    }
}

static void test_all_white(void) {
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_WHITE, true);
}

static void test_white_frame(void) {
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_BLACK, true);
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_WHITE, false);
}

static void test_black_frame(void) {
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_WHITE, true);
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_BLACK, false);
}

static void test_all_black(void) {
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_BLACK, true);
}

static void test_fill_horz(uint16_t offset) {
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_BLACK, true);
    for (uint16_t y = offset; y < screen_h; y += 2) {
        qp_rect(screen, 0, y, screen_w - 1, y, HSV_WHITE, true);
    }
}

static void test_fill_vert(uint16_t offset) {
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_BLACK, true);
    for (uint16_t x = offset; x < screen_w; x += 2) {
        qp_rect(screen, x, 0, x, screen_h - 1, HSV_WHITE, true);
    }
}

static void test_checkerboard(uint16_t block_size) {
    uint16_t screen_w = qp_get_width(screen);
    uint16_t screen_h = qp_get_height(screen);
    uint16_t cols     = (screen_w + block_size - 1) / block_size;
    uint16_t rows     = (screen_h + block_size - 1) / block_size;
    qp_rect(screen, 0, 0, screen_w - 1, screen_h - 1, HSV_BLACK, true);
    for (uint16_t row = 0; row < rows; ++row) {
        for (uint16_t col = 0; col < cols; ++col) {
            if ((row + col) % 2 == 0) {
                uint16_t l = col * block_size;
                uint16_t r = l + block_size - 1;
                uint16_t t = row * block_size;
                uint16_t b = t + block_size - 1;
                if (r >= screen_w) r = screen_w - 1;
                if (b >= screen_h) b = screen_h - 1;
                qp_rect(screen, l, t, r, b, HSV_WHITE, true);
            }
        }
    }
}

#include "graphics/6x9.qff.h"
#include "graphics/6x13.qff.h"
#include "graphics/8x13.qff.h"
#include "graphics/9x15.qff.h"
#include "graphics/10x20.qff.h"

static const void *const font_data[] = {
    [FONT_6x9] = font_6x9, [FONT_6x13] = font_6x13, [FONT_8x13] = font_8x13, [FONT_9x15] = font_9x15, [FONT_10x20] = font_10x20,
};

static void load_font(void) {
    if (font) {
        qp_close_font(font);
    }
    font = qp_load_font_mem(font_data[font_size]);
}

void keyboard_post_init_user(void) {
    load_font();
    screen = make_screen();
    qp_init(screen, rotation);
}

static void dance_qp_finished(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:
            if (state->pressed) {
                // single hold - step through rotations
                switch (rotation) {
                    case QP_ROTATION_0:
                        rotation = QP_ROTATION_90;
                        break;
                    case QP_ROTATION_90:
                        rotation = QP_ROTATION_180;
                        break;
                    case QP_ROTATION_180:
                        rotation = QP_ROTATION_270;
                        break;
                    default:
                        rotation = QP_ROTATION_0;
                        break;
                }
                qp_init(screen, rotation);
            } else {
                // single tap - step through test modes
                if (test_mode < TEST_LAST) {
                    ++test_mode;
                } else {
                    test_mode = TEST_FIRST;
                }
            }
            restart_test = true;
            need_update  = true;
            break;
        case 2:
            if (state->pressed) {
                // tap+hold - step through font sizes
                if (font_size < FONT_LAST) {
                    ++font_size;
                } else {
                    font_size = FONT_FIRST;
                }
                load_font();
            }
            restart_test = true;
            need_update  = true;
            break;
        case 3:
            if (!state->pressed) {
                // triple tap - toggle update speed test
                update_speed_test = !update_speed_test;
                if (update_speed_test) {
                    update_speed_start_timer = timer_read32();
                    update_speed_count       = 0;
                }
            }
            break;
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_QP] = ACTION_TAP_DANCE_FN(dance_qp_finished),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT_ortho_1x1(TD(TD_QP)),
};

void housekeeping_task_user(void) {
    if (update_speed_test) {
        // Update statistics and send the measurement result to the console.
        if (update_speed_count && (update_speed_count % 256 == 0)) {
            uprintf("Painter: %u updates, %lu ms\n", update_speed_count, timer_elapsed32(update_speed_start_timer));
        }
        update_speed_count++;

        // Toggle between the "all white" and "all black" states and trigger the screen update.
        if (test_mode == TEST_ALL_WHITE) {
            test_mode = TEST_ALL_BLACK;
        } else {
            test_mode = TEST_ALL_WHITE;
        }
        need_update = true;
    }

    if (!need_update) {
        switch (test_mode) {
            case TEST_SLOW_UPDATE_ON_BLACK:
            case TEST_SLOW_UPDATE_ON_WHITE:
                break;
            default:
                return;
        }
    }
    need_update = false;

    switch (test_mode) {
        case TEST_COLOR_GRID:
            test_color_grid();
            break;
        case TEST_CHARACTERS_ON_BLACK:
            test_characters(false);
            break;
        case TEST_CHARACTERS_ON_WHITE:
            test_characters(true);
            break;
        case TEST_SLOW_UPDATE_ON_BLACK:
            test_slow_update(false);
            break;
        case TEST_SLOW_UPDATE_ON_WHITE:
            test_slow_update(true);
            break;
        case TEST_ALL_WHITE:
            test_all_white();
            break;
        case TEST_WHITE_FRAME:
            test_white_frame();
            break;
        case TEST_BLACK_FRAME:
            test_black_frame();
            break;
        case TEST_ALL_BLACK:
            test_all_black();
            break;
        case TEST_FILL_HORZ_0:
            test_fill_horz(0);
            break;
        case TEST_FILL_HORZ_1:
            test_fill_horz(1);
            break;
        case TEST_FILL_VERT_0:
            test_fill_vert(0);
            break;
        case TEST_FILL_VERT_1:
            test_fill_vert(1);
            break;
        case TEST_FILL_CHECKERBOARD_1:
            test_checkerboard(1);
            break;
        case TEST_FILL_CHECKERBOARD_2:
            test_checkerboard(2);
            break;
        case TEST_FILL_CHECKERBOARD_4:
            test_checkerboard(4);
            break;
    }
    qp_flush(screen);
}
