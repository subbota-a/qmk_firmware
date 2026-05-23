/*
 * Keymap-specific config overrides.
 *
 * Default RGB matrix state on fresh EEPROM: solid white at maximum
 * brightness, no animation.
 */

#pragma once

// Compile in the SOLID_COLOR animation. Keychron's keyboard.json does not
// list it, so we explicitly enable it for this keymap.
#define ENABLE_RGB_MATRIX_SOLID_COLOR

// Defaults applied on fresh EEPROM (or when EECONFIG magic number mismatches
// and QMK reinitialises RGB settings). After that, user changes are stored
// in EEPROM and override these.
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR
#define RGB_MATRIX_DEFAULT_HUE  0    // ignored when SAT=0
#define RGB_MATRIX_DEFAULT_SAT  0    // 0 saturation = pure white
#define RGB_MATRIX_DEFAULT_VAL  255  // maximum brightness
