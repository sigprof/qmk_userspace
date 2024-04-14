// Copyright 2024 Sergey Vlasov (@sigprof)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _NUMPD,     // Numpad keycodes
    _NROW,      // Numpad layout with number row keycodes
    _F0112,     // F1 ... F12
    _F1324,     // F13 ... F24
    _L4,        // spare
    _L5,        // spare
    _L6,        // spare
    _ADJ,       // Configuration
};

// Shorter defines for some QMK keycodes (to keep the keymap aligned)
#define U_LTESC LT(_ADJ, KC_ESC)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Numpad keycodes
    [_NUMPD] = LAYOUT_top(
            KC_HOME,      KC_MUTE,     KC_MPLY,
        U_LTESC, KC_P7,   KC_P8,   KC_P9,   KC_BSPC,
        KC_TAB,  KC_P4,   KC_P5,   KC_P6,   KC_PENT,
        KC_P0,   KC_P1,   KC_P2,   KC_P3,   KC_PDOT
    ),

    // Number row keycodes
    [_NROW] = LAYOUT_top(
            _______,      _______,     _______,
        _______, KC_7,    KC_8,    KC_9,    _______,
        _______, KC_4,    KC_5,    KC_6,    KC_ENT,
        KC_0,    KC_1,    KC_2,    KC_3,    KC_DOT
    ),

    // F1 ... F12
    [_F0112] = LAYOUT_top(
            _______,      _______,     _______,
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,
        _______, KC_F5,   KC_F6,   KC_F7,   KC_F8,
        KC_PSCR, KC_F9,   KC_F10,  KC_F11,  KC_F12
    ),

    // F13 ... F24
    [_F1324] = LAYOUT_top(
            _______,      _______,     _______,
        _______, KC_F13,  KC_F14,  KC_F15,  KC_F16,
        _______, KC_F17,  KC_F18,  KC_F19,  KC_F20,
        KC_PSCR, KC_F21,  KC_F22,  KC_F23,  KC_F24
    ),

    [_L4] = LAYOUT_top(
            _______,      _______,     _______,
        _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______
    ),

    [_L5] = LAYOUT_top(
            _______,      _______,     _______,
        _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______
    ),

    [_L6] = LAYOUT_top(
            _______,      _______,     _______,
        _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______
    ),

    // Configuration
    [_ADJ] = LAYOUT_top(
            RGB_M_P,      RGB_M_B,     QK_BOOT,
        _______, RGB_TOG, KC_PSLS, KC_PAST, KC_PMNS,
        KC_NUM,  TO(4),   TO(5),   TO(6),   KC_PPLS,
        TO(0),   TO(1),   TO(2),   TO(3),   _______
    ),
};
// clang-format on

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_NUMPD] =   { ENCODER_CCW_CW(KC_WH_L, KC_WH_R), ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_WH_U, KC_WH_D) },
    [1 ... 6] =  { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
    [_ADJ] =     { ENCODER_CCW_CW(RGB_HUD, RGB_HUI), ENCODER_CCW_CW(RGB_SAD, RGB_SAI), ENCODER_CCW_CW(RGB_VAD, RGB_VAI) },
};

bool rgb_matrix_indicators_user(void) {
    if (layer_state_is(_ADJ)) {
        if (host_keyboard_led_state().num_lock) {
            rgb_matrix_set_color(7, RGB_GREEN);
        }

        uint8_t layer_indicator;
        if (layer_state_is(6)) {
            layer_indicator = 16;
        } else if (layer_state_is(5)) {
            layer_indicator = 13;
        } else if (layer_state_is(4)) {
            layer_indicator = 10;
        } else if (layer_state_is(3)) {
            layer_indicator = 15;
        } else if (layer_state_is(2)) {
            layer_indicator = 14;
        } else if (layer_state_is(1)) {
            layer_indicator = 9;
        } else {
            layer_indicator = 8;
        }
        rgb_matrix_set_color(layer_indicator, RGB_RED);
    }
    return false;
}
