 /* Copyright 2020 Naoki Katahira
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
#include <stdio.h>
#include "os_detection.h"

#pragma region Layers
enum layer_number {
  _QWERTY = 0,
  _SYMBOLS,
  _NAV,
  _MOUSE,
  _GAMING,
  _UNUSED,
};

#define RAISE MO(_NAV)
#define LOWER MO(_SYMBOLS)

/*
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------.    ,-------|      |      |      |      |      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |      |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |LOWER | LGUI | Alt  | /Space  /       \Enter \  |BackSP| RGUI |RAISE |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
#define C_S(kc) LCTL(LSFT(kc))

// Custom chat mode keycode: press to switch into QWERTY so you can type chat,
// press Escape to return to the previous layer (e.g. GAMING).
enum custom_keycodes {
    CHAT_MODE = QK_KB_0,
};

static uint8_t prev_layer = _QWERTY;
static bool chat_active = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_QWERTY] = LAYOUT(
  KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_GRV,
  LT(1,KC_TAB), KC_Q, KC_W,  KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
  KC_LCTL, LGUI_T(KC_A), LALT_T(KC_S), LSFT_T(KC_D), LCTL_T(KC_F), KC_G, KC_H, RCTL_T(KC_J), RSFT_T(KC_K), RALT_T(KC_L), RGUI_T(KC_SCLN), KC_QUOT,
  KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B, KC_LBRC,  KC_RBRC,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  KC_RSFT,
                      KC_LALT, KC_NO,   MO(1),   KC_SPC, LT(3,KC_ENT), KC_BSPC, KC_BSPC, MO(2)
),
[_SYMBOLS] = LAYOUT(
  _______, _______, _______, _______, _______, _______,                   _______, _______, _______,_______, _______, _______,
  _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______,
  _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, KC_PIPE, KC_GRAVE, KC_PLUS, KC_LCBR, KC_RCBR, _______,
                             _______, _______, _______, _______, _______,  _______, _______, _______
),
[_NAV] = LAYOUT(
  _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                       KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  _______, _______, _______, _______, _______, _______,                     XXXXXXX, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX,
  _______, _______, _______, _______, _______, _______,   _______, _______, KC_PLUS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS, _______,
                             _______, _______, _______,  _______, _______,  _______, _______, _______
),
[_MOUSE] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                             _______, _______, _______, _______, _______,  _______, _______, _______
),
[_UNUSED] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                             _______, _______, _______, _______, _______,  _______, _______, _______
),
[_GAMING] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                             CHAT_MODE, _______, _______, _______, _______,  _______, _______, _______
)
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == CHAT_MODE) {
        if (record->event.pressed) {
            prev_layer = get_highest_layer(layer_state | default_layer_state);
            layer_move(_QWERTY);
            chat_active = true;
        }
        return false; // don't send any keypress for CHAT_MODE itself
    }

    // If ESC or Enter is pressed/released while in chat mode, return to previous layer
    if (keycode == KC_ESC) {
        if (record->event.pressed) {
            if (chat_active) {
                layer_move(prev_layer);
                chat_active = false;
            }
        }
        return true;
    }

    return true;
}

#pragma endregion

#pragma region OS_detection
bool process_detected_host_os_kb(os_variant_t detected_os) {
    switch (detected_os) {
        case OS_MACOS:
            keymap_config.swap_lctl_lgui = true;
            keymap_config.swap_rctl_rgui = true;
            break;
        case OS_WINDOWS:
        case OS_LINUX:
        default:
            keymap_config.swap_lctl_lgui = false;
            keymap_config.swap_rctl_rgui = false;
            break;
    }
    return true;
}

#pragma endregion

#pragma region OLED
#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    // Vertical orientation for both screens.
    return OLED_ROTATION_270;
}

void render_space(void) {
    oled_write_P(PSTR("     "), false);
}

void render_mod_status_gui_alt(uint8_t modifiers) {
    static const char PROGMEM gui_off_1[] = {0x80, 0x81, 0};
    static const char PROGMEM gui_off_2[] = {0xa0, 0xa1, 0};
    static const char PROGMEM gui_on_1[]  = {0x88, 0x89, 0};
    static const char PROGMEM gui_on_2[]  = {0xa8, 0xa9, 0};

    static const char PROGMEM alt_off_1[] = {0x82, 0x83, 0};
    static const char PROGMEM alt_off_2[] = {0xa2, 0xa3, 0};
    static const char PROGMEM alt_on_1[]  = {0x8a, 0x8b, 0};
    static const char PROGMEM alt_on_2[]  = {0xaa, 0xab, 0};

    // fillers between the modifier icons bleed into the icon frames
    static const char PROGMEM off_off_1[] = {0xc0, 0};
    static const char PROGMEM off_off_2[] = {0xc1, 0};
    static const char PROGMEM on_off_1[]  = {0xc2, 0};
    static const char PROGMEM on_off_2[]  = {0xc3, 0};
    static const char PROGMEM off_on_1[]  = {0xc4, 0};
    static const char PROGMEM off_on_2[]  = {0xc5, 0};
    static const char PROGMEM on_on_1[]   = {0xc6, 0};
    static const char PROGMEM on_on_2[]   = {0xc7, 0};

    if (modifiers & MOD_MASK_GUI) {
        oled_write_P(gui_on_1, false);
    } else {
        oled_write_P(gui_off_1, false);
    }

    if ((modifiers & MOD_MASK_GUI) && (modifiers & MOD_MASK_ALT)) {
        oled_write_P(on_on_1, false);
    } else if (modifiers & MOD_MASK_GUI) {
        oled_write_P(on_off_1, false);
    } else if (modifiers & MOD_MASK_ALT) {
        oled_write_P(off_on_1, false);
    } else {
        oled_write_P(off_off_1, false);
    }

    if (modifiers & MOD_MASK_ALT) {
        oled_write_P(alt_on_1, false);
    } else {
        oled_write_P(alt_off_1, false);
    }

    if (modifiers & MOD_MASK_GUI) {
        oled_write_P(gui_on_2, false);
    } else {
        oled_write_P(gui_off_2, false);
    }

    if ((modifiers & MOD_MASK_GUI) && (modifiers & MOD_MASK_ALT)) {
        oled_write_P(on_on_2, false);
    } else if (modifiers & MOD_MASK_GUI) {
        oled_write_P(on_off_2, false);
    } else if (modifiers & MOD_MASK_ALT) {
        oled_write_P(off_on_2, false);
    } else {
        oled_write_P(off_off_2, false);
    }

    if (modifiers & MOD_MASK_ALT) {
        oled_write_P(alt_on_2, false);
    } else {
        oled_write_P(alt_off_2, false);
    }
}

void render_mod_status_ctrl_shift(uint8_t modifiers) {
    static const char PROGMEM ctrl_off_1[] = {0x84, 0x85, 0};
    static const char PROGMEM ctrl_off_2[] = {0xa4, 0xa5, 0};
    static const char PROGMEM ctrl_on_1[]  = {0x8c, 0x8d, 0};
    static const char PROGMEM ctrl_on_2[]  = {0xac, 0xad, 0};

    static const char PROGMEM shift_off_1[] = {0x86, 0x87, 0};
    static const char PROGMEM shift_off_2[] = {0xa6, 0xa7, 0};
    static const char PROGMEM shift_on_1[]  = {0xc8, 0xc9, 0};
    static const char PROGMEM shift_on_2[]  = {0xca, 0xcb, 0};

    // fillers between the modifier icons bleed into the icon frames
    static const char PROGMEM off_off_1[] = {0xc0, 0};
    static const char PROGMEM off_off_2[] = {0xc1, 0};
    static const char PROGMEM on_off_1[]  = {0xc2, 0};
    static const char PROGMEM on_off_2[]  = {0xc3, 0};
    static const char PROGMEM off_on_1[]  = {0xc4, 0};
    static const char PROGMEM off_on_2[]  = {0xc5, 0};
    static const char PROGMEM on_on_1[]   = {0xc6, 0};
    static const char PROGMEM on_on_2[]   = {0xc7, 0};

    if (modifiers & MOD_MASK_CTRL) {
        oled_write_P(ctrl_on_1, false);
    } else {
        oled_write_P(ctrl_off_1, false);
    }

    if ((modifiers & MOD_MASK_CTRL) && (modifiers & MOD_MASK_SHIFT)) {
        oled_write_P(on_on_1, false);
    } else if (modifiers & MOD_MASK_CTRL) {
        oled_write_P(on_off_1, false);
    } else if (modifiers & MOD_MASK_SHIFT) {
        oled_write_P(off_on_1, false);
    } else {
        oled_write_P(off_off_1, false);
    }

    if (modifiers & MOD_MASK_SHIFT) {
        oled_write_P(shift_on_1, false);
    } else {
        oled_write_P(shift_off_1, false);
    }

    if (modifiers & MOD_MASK_CTRL) {
        oled_write_P(ctrl_on_2, false);
    } else {
        oled_write_P(ctrl_off_2, false);
    }

    if ((modifiers & MOD_MASK_CTRL) && (modifiers & MOD_MASK_SHIFT)) {
        oled_write_P(on_on_2, false);
    } else if (modifiers & MOD_MASK_CTRL) {
        oled_write_P(on_off_2, false);
    } else if (modifiers & MOD_MASK_SHIFT) {
        oled_write_P(off_on_2, false);
    } else {
        oled_write_P(off_off_2, false);
    }

    if (modifiers & MOD_MASK_SHIFT) {
        oled_write_P(shift_on_2, false);
    } else {
        oled_write_P(shift_off_2, false);
    }
}

void render_paw(void) {
    static const char PROGMEM paw[] = {0x8e, 0x8f, 0x90, 0x91, 0x92, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0};
    oled_write_P(paw, false);
}

void render_qr(void) {
    // clang-format off
    static const char PROGMEM qr[] = {
        0x93, 0x94, 0x95, 0x96, 0x97,
        0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
        0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0,
    };
    // clang-format on
    oled_write_P(qr, false);
    // oled_write_P(PSTR("don't"), false);
}

void render_layer_state(void) {
    oled_write_P(PSTR("Layer"), false);
    // Be aware that length of layer captions should be always 5 characters, otherwise the layout will look eerie.
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("QWERT"), false);
            break;
        case _SYMBOLS:
            oled_write_P(PSTR("SYM  "), false);
            break;
        case _NAV:
            oled_write_P(PSTR("NAV  "), false);
            break;
        case _MOUSE:
            oled_write_P(PSTR("MOUSE"), false);
            break;
        case _GAMING:
            oled_write_P(PSTR("GAME "), false);
            break;
        case _UNUSED:
            oled_write_P(PSTR("WIP  "), false);
            break;
        default:
            oled_write_P(PSTR("?????"), false);
            break;
    }
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        render_paw();
        render_space();
        render_space();
        render_layer_state();
        render_space();
        render_space();
        render_mod_status_gui_alt(get_mods() | get_oneshot_mods());
        render_mod_status_ctrl_shift(get_mods() | get_oneshot_mods());
    } else {
        render_paw();
        render_space();
        render_space();
        render_qr();
    }

    return false;
}

#endif // OLED_ENABLE

#pragma endregion
