/* Copyright 2020 Sergey Vlasov <sigprof@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#include "sigprof.h"

enum layer_names {
    _QWERTY,
    _NUMPAD,
    _FN,
    _FN_CTL,
    _ADJUST,
};

enum tap_dance_ids {
    TD_RALT,
    TD_RCTL,
    TD_LSFT,
    TD_RSFT,
};

#define U_CESC  MT(MOD_LCTL, KC_ESC)
#define U_FAPP  LT(_FN, KC_APP)
#define U_FN    MO(_FN)
#define U_TRALT TD(TD_RALT)
#define U_TRCTL TD(TD_RCTL)
#define U_TLSFT TD(TD_LSFT)
#define U_TRSFT TD(TD_RSFT)
#define U_NBSLS LT(_NUMPAD, KC_BSLS)
#define U_MOADJ OSL(_ADJUST) //MO(_ADJUST)
#define U_TGNUM TG(_NUMPAD)
#define U_OSRGU OSM(MOD_RGUI)
#define U_OSRCT OSM(MOD_RCTL)
#define U_CPGUP RCTL(KC_PGUP)
#define U_CPGDN RCTL(KC_PGDN)
#define U_FNCTL LM(_FN_CTL, MOD_LCTL)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ANSI QWERTY keyboard adapted to the ID80 physical layout.
     *
     * Two keys in the right column are used as PgUp and PgDn (using the top
     * row key as PgDn turned out to be really inconvenient).  Two keys in the
     * top row (separated by 0.25U) are used as Ins and Del (although the Del
     * key is located far from the main block, reversing the order of the
     * Ins/Del pair does not seem to be good).
     *
     * There are only two modifier keys on the right side of the spacebar,
     * which perform multiple functions through tap dances; here they are
     * labeled as “TRAlt” and “TRCtl”, but the primary function of the “TRCtl”
     * key is switching to the Fn layer.
     *
     * On the left side the Caps Lock key is also overloaded to work as Ctrl on
     * hold and Esc on tap (the Ctrl modifier location is similar to HHKB, and
     * the replacement of the tap action with Esc is done for compatibility
     * with my custom layouts for some smaller keyboards which also have the
     * Esc key in that location).  The bottom left key (originally Ctrl) is
     * repurposed as the Fn layer switch key.
     *
     * ┌───┐┌───┬───┬───┬───┐┌───┬───┬───┬───┐┌───┬───┬───┬───┐┌───┐┌───┐
     * │Esc││F1 │F2 │F3 │F4 ││F5 │F6 │F7 │F8 ││F9 │F10│F11│F12││Ins││Del│
     * └───┘└───┴───┴───┴───┘└───┴───┴───┴───┘└───┴───┴───┴───┘└───┘└───┘
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐┌───┐
     * │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │   \   ││PgU│
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤├───┤
     * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │BkSp ││PgD│
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤└───┘
     * │EscCtl│ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │  Enter │
     * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┘
     * │ ShiftL │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │ShiftL│┌───┐
     * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┴┬──┴──┬───┘│ ↑ │
     * │ Fn │GUI │Alt │                        │TRAlt│TRCtl│┌───┼───┼───┐
     * └────┴────┴────┴────────────────────────┴─────┴─────┘│ ← │ ↓ │ → │
     *                                                      └───┴───┴───┘
     */

    [_QWERTY] = LAYOUT_ansi(
        KC_ESC,    KC_F1,   KC_F2,   KC_F3,   KC_F4,     KC_F5,   KC_F6,   KC_F7,   KC_F8,     KC_F9,   KC_F10,  KC_F11,  KC_F12,     KC_INS,    KC_DEL,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  U_NBSLS,            KC_PGUP,
        KC_TAB,      KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,        KC_PGDN,
        U_CESC,        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
        U_TLSFT,            KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, U_TRSFT,          KC_UP,
        U_FN,      KC_LGUI,   KC_LALT,                       KC_SPC,                              U_TRALT,     U_TRCTL,        KC_LEFT, KC_DOWN, KC_RGHT
    ),

    /*
     * Numeric keypad emulation layer.
     * Unused alphanumeric keys are turned off.
     */
    [_NUMPAD] = LAYOUT_ansi(
        _______,   _______, _______, _______, _______,   _______, _______, _______, _______,   _______, _______, _______, _______,    _______,   _______,
        XXXXXXX, KC_P1,   KC_P2,   KC_P3,   KC_P4,   KC_P5,   KC_P6,   KC_P7,   KC_P8,   KC_P9,   KC_P0,   KC_PMNS, KC_PPLS, _______,            _______,
        _______,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_P4,   KC_P5,   KC_P6,   KC_PAST, KC_PENT, KC_NUM,  KC_NUM,         _______,
        _______,       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_P1,   KC_P2,   KC_P3,   KC_PPLS, KC_PAST, KC_PENT,
        _______,            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_P0,   KC_PCMM, KC_PDOT, KC_PSLS, _______,          _______,
        _______,   _______,   _______,                      _______,                              _______,     _______,        _______, _______, _______
    ),

    /*
     * Function layer:
     * - mouse emulation
     * - cursor and nav cluster keys (for compatibility with 60%)
     * - one-shot RCtrl and RGUI
     * - Num Lock key
     * - toggle number pad emulation layer
     * - momentary switch for the number pad emulation layer (because of layer
     *   ordering, using that switch requires releasing the key which was used
     *   to switch to the function layer)
     * - media keys
     */
    [_FN] = LAYOUT_ansi(
        U_BTN1R,   KC_F13,  KC_F14,  KC_F15,  KC_F16,    KC_F17,  KC_F18,  KC_F19,  KC_F20,    KC_F21,  KC_F22,  KC_F23,  KC_F24,     KC_HOME,   KC_END,
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,             U_CPGUP,
        KC_CAPS,     MS_BTN1, MS_UP,   MS_BTN2, MS_WHLU, DM_PLY1, KC_INS,  KC_HOME, KC_UP,   KC_END,  KC_PGUP, KC_VOLU, KC_MUTE, U_TGNUM,        U_CPGDN,
        U_FNCTL,       MS_LEFT, MS_DOWN, MS_RGHT, MS_WHLD, DM_PLY2, KC_DEL,  KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_VOLD, KC_PENT,
        _______,            MS_BTN3, MS_BTN4, MS_BTN5, MS_WHLL, MS_WHLR, KC_PSCR, KC_SCRL, KC_PAUS, U_OSRGU, U_OSRCT, _______,          KC_PGUP,
        _______,   _______,   _______,                      U_MOADJ,                              _______,     _______,        KC_HOME, KC_PGDN, KC_END
    ),

    /*
     * Fn+Ctrl layer - dynamic macro management:
     * - Fn+Ctrl+Esc - stop recording;
     * - Fn+Ctrl+T   - start recording macro 1;
     * - Fn+Ctrl+G   - start recording macro 2.
     */
    [_FN_CTL] = LAYOUT_ansi(
        DM_RSTP,   _______, _______, _______, _______,   _______, _______, _______, _______,   _______, _______, _______, _______,    _______,   _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______,
        _______,     _______, _______, _______, _______, DM_REC1, _______, _______, _______, _______, _______, _______, _______, _______,        _______,
        _______,       _______, _______, _______, _______, DM_REC2, _______, _______, _______, _______, _______, _______, _______,
        _______,            _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______,   _______,   _______,                      _______,                              _______,     _______,        _______, _______, _______
    ),

    /*
     * Adjust layer:
     * - backlight control
     * - RGB underglow control
     * - reset to bootloader
     * - EEPROM reset
     * - NKRO config
     */
    [_ADJUST] = LAYOUT_ansi(
        QK_BOOT,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX, NK_OFF,  NK_ON,   DB_TOGG,    XXXXXXX,   XXXXXXX,
        XXXXXXX, U_LSWM0, U_LSWM1, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, NK_TOGG, EE_CLR,             XXXXXXX,
        XXXXXXX,     BL_TOGG, BL_DOWN, BL_UP,   BL_BRTG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, BS_TOGG,        XXXXXXX,
        _______,       UG_TOGG, UG_NEXT, UG_HUEU, UG_SATU, UG_VALU, UG_SPDU, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _______,            RGB_M_P, UG_PREV, UG_HUED, UG_SATD, UG_VALD, UG_SPDD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,          XXXXXXX,
        _______,   _______,   _______,                      _______,                              _______,     _______,        XXXXXXX, XXXXXXX, XXXXXXX
    ),
};

enum rgblight_layer_ids {
    RGBL_NUMPAD,
};

const rgblight_segment_t PROGMEM numpad_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {  0, 1, HSV_AZURE },
    {  6, 3, HSV_AZURE },
    { 14, 2, HSV_AZURE }
);

const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    numpad_layer
);

static struct {
    bool group_led : 1;
} indicators;

static void update_indicators(void) {
    if (indicators.group_led) {
        backlight_set(get_backlight_level());
    } else {
        backlight_set(0);
    }
}

void keyboard_post_init_keymap(void) {
    rgblight_layers = my_rgb_layers;
    rgblight_set_effect_range(0, 16);
    update_indicators();
}

bool led_update_user(led_t led_state) {
    // Assume that "grp_led:scroll" is used to make the Scroll Lock LED
    // indicate the input language (XKB group).
    if (indicators.group_led != led_state.scroll_lock) {
        indicators.group_led = led_state.scroll_lock;
        update_indicators();
    }

    // Keep the default Caps Lock LED function
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(RGBL_NUMPAD, layer_state_cmp(state, _NUMPAD));
    return state;
}

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case U_CESC:
        case U_FAPP:
            return true;

        default:
            return false;
    }
}

static const uint16_t PROGMEM combo_prtsc[] = { KC_F9, KC_F10, COMBO_END };
static const uint16_t PROGMEM combo_scrlk[] = { KC_F10, KC_F11, COMBO_END };
static const uint16_t PROGMEM combo_pause[] = { KC_F11, KC_F12, COMBO_END };
static const uint16_t PROGMEM combo_fapp[]  = { KC_INS, KC_DEL, COMBO_END };

combo_t key_combos[] = {
    COMBO(combo_prtsc, KC_PSCR),
    COMBO(combo_scrlk, KC_SCRL),
    COMBO(combo_pause, KC_PAUS),
    COMBO(combo_fapp,  U_FAPP),
};

uint16_t COMBO_LEN = sizeof(key_combos) / sizeof(key_combos[0]);

// Tap dance for the “Right Alt” key:
// - hold: KC_RALT
// - tap: KC_RALT
// - tap and hold: KC_RGUI
// - double tap: KC_RGUI
// - double tap and hold: KC_RGUI+KC_RALT
// - triple tap: KC_RGUI+KC_RALT
enum td_ralt_state {
    TD_RALT_NOOP,
    TD_RALT_RALT,
    TD_RALT_RGUI,
    TD_RALT_RGUI_RALT,
};

static enum td_ralt_state td_ralt_state;

static void td_ralt_finished(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:
            td_ralt_state = TD_RALT_RALT;
            break;
        case 2:
            td_ralt_state = TD_RALT_RGUI;
            break;
        case 3:
            td_ralt_state = TD_RALT_RGUI_RALT;
            break;
        default:
            td_ralt_state = TD_RALT_NOOP;
            break;
    }

    switch (td_ralt_state) {
        case TD_RALT_NOOP:
            break;
        case TD_RALT_RALT:
            register_code(KC_RALT);
            break;
        case TD_RALT_RGUI:
            register_code(KC_RGUI);
            break;
        case TD_RALT_RGUI_RALT:
            register_code(KC_RGUI);
            register_code(KC_RALT);
            break;
    }
}

static void td_ralt_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_ralt_state) {
        case TD_RALT_NOOP:
            break;
        case TD_RALT_RALT:
            unregister_code(KC_RALT);
            break;
        case TD_RALT_RGUI:
            unregister_code(KC_RGUI);
            break;
        case TD_RALT_RGUI_RALT:
            unregister_code(KC_RALT);
            unregister_code(KC_RGUI);
            break;
    }
}

// Tap dance for the “Right Ctrl” key:
// - hold: MO(_FN)
// - tap: KC_APP
// - tap and hold: KC_RCTL
// - double tap: KC_RCTL
// - double tap and hold: KC_APP
// - triple tap: KC_APP
enum td_rctl_state {
    TD_RCTL_NOOP,
    TD_RCTL_MO_FN,
    TD_RCTL_APP,
    TD_RCTL_RCTL,
};

static enum td_rctl_state td_rctl_state;

static void td_rctl_finished(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:
            if (state->pressed) {
                td_rctl_state = TD_RCTL_MO_FN;
            } else {
                td_rctl_state = TD_RCTL_APP;
            };
            break;
        case 2:
            td_rctl_state = TD_RCTL_RCTL;
            break;
        case 3:
            td_rctl_state = TD_RCTL_APP;
            break;
        default:
            td_rctl_state = TD_RCTL_NOOP;
            break;
    }

    switch (td_rctl_state) {
        case TD_RCTL_NOOP:
            break;
        case TD_RCTL_MO_FN:
            layer_on(_FN);
            break;
        case TD_RCTL_APP:
            register_code(KC_APP);
            break;
        case TD_RCTL_RCTL:
            register_code(KC_RCTL);
            break;
    }
}

static void td_rctl_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_rctl_state) {
        case TD_RCTL_NOOP:
            break;
        case TD_RCTL_MO_FN:
            layer_off(_FN);
            break;
        case TD_RCTL_APP:
            unregister_code(KC_APP);
            break;
        case TD_RCTL_RCTL:
            unregister_code(KC_RCTL);
            break;
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_RALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_ralt_finished, td_ralt_reset),
    [TD_RCTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_rctl_finished, td_rctl_reset),
    [TD_LSFT] = ACTION_LANG_SWITCH_LSFT,
    [TD_RSFT] = ACTION_LANG_SWITCH_RSFT,
};

void eeconfig_init_user(void) {
    // RGB Lighting needs to be enabled for the lighting layers to work;
    // however, the default color may be set to black.
    rgblight_enable();
    rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_sethsv(HSV_BLACK);

    // The backlight level needs to be set, but the backlight itself should be
    // disabled (it would be enabled with the saved level by the group
    // indicator code).
    backlight_level(BACKLIGHT_LEVELS / 3);
    backlight_disable();
}

/* vim:set sw=4 sta et: */
