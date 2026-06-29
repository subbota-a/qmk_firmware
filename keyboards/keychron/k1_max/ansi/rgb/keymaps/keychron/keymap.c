/* Copyright 2024 ~ 2026 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "keychron_common.h"

enum layers {
    QWERTY  = 0,
    _FN     = 1,
    COLEMAK = 2,
};

#define FN_KEY MO(_FN)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [QWERTY] = LAYOUT_ansi_87(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  UG_TOGG, KC_SYSTEM_POWER,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RGUI,  FN_KEY,   KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [_FN] = LAYOUT_ansi_87(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_CALCULATOR,  UG_VALD,  UG_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  _______,  _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______),

    [COLEMAK] = LAYOUT_ansi_87(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  KC_Q,  KC_W,  KC_F,     KC_P,     KC_B,     KC_J,     KC_L,     KC_U,     KC_Y,     KC_SCLN,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  KC_A,  KC_R,     KC_S,     KC_T,     KC_G,  KC_M,     KC_N,     KC_E,     KC_I,     KC_O,     _______,            _______,
        _______,            KC_X,     KC_C,     KC_D,     KC_V,  KC_Z,     KC_K,     KC_H,     _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______)
};

// clang-format on

// === Colemak auto-activation logic ===
//
// COLEMAK overlay activates on top of QWERTY only when ALL of:
//   - colemak_allowed: dip switch in Mac position
//   - !russian_mode:   Scroll Lock LED off (= English XKB active)
//   - !any_mod_held:   no Ctrl/Alt/GUI held (shortcuts stay on QWERTY)
//   - !fn_held:        Fn-layer not active (Fn-functions stay stock-style)
//
// Single source of truth is layer_state_set_user — it gates the COLEMAK bit
// based on the flags above. Other callbacks set the flags and trigger
// re-evaluation via layer_state_set(layer_state).

static bool colemak_allowed = false;
static bool russian_mode    = false;
static bool any_mod_held    = false;

static void reapply_layers(void) {
    layer_state_set(layer_state);
}

// Strong override of weak default in k1_max.c — invoked from
// dip_switch_update_kb when the slide switch moves.
// Mac position (active=true)  → Colemak overlay allowed.
// Win position (active=false) → Colemak disabled (pure stock QWERTY).
void on_mac_windows_switch(bool active) {
    default_layer_set(1UL << QWERTY);
    colemak_allowed = active;
    reapply_layers();
}

// Gate the COLEMAK overlay. Called on every overlay-layer change, including
// our reapply_layers() invocations.
layer_state_t layer_state_set_user(layer_state_t state) {
    bool fn_held = (state & (1UL << _FN)) != 0;
    bool should_colemak = colemak_allowed
                       && !russian_mode
                       && !any_mod_held
                       && !fn_held;
    if (should_colemak) {
        state |= (1UL << COLEMAK);
    } else {
        state &= ~(1UL << COLEMAK);
    }
    return state;
}

// Plasma's grp_led:scroll binds Scroll Lock LED to active XKB group.
// LED off (group 0 = us) → russian_mode=false → Colemak engaged.
// LED on  (group 1 = ru) → russian_mode=true  → user types standard йцукен
//                                               via QWERTY scancodes.
bool led_update_user(led_t led_state) {
    russian_mode = led_state.scroll_lock;
    reapply_layers();
    return true;
}

// Track Ctrl/Alt/GUI state. Shortcuts stay on QWERTY positions by
// disengaging Colemak when any modifier is held. get_mods() reflects state
// AFTER QMK has processed the press/release.
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t mods = get_mods();
    bool new_state = (mods & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI)) != 0;
    if (new_state != any_mod_held) {
        any_mod_held = new_state;
        reapply_layers();
    }
}
