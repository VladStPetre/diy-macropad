#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
     KC_1, KC_2, KC_3, KC_MUTE,
     KC_4, KC_5, KC_6
    ),

    [1] = LAYOUT(
     _______, _______, _______, _______,
     _______, _______, _______
    ),

    [2] = LAYOUT(
     _______, _______, _______, _______,
     _______, _______, _______
    ),

    [3] = LAYOUT(
     _______, _______, _______, _______,
     _______, _______, _______
    )
    };

#if defined(ENCODER_MAP_ENABLE)

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = { ENCODER_CCW_CW(MS_WHLU, MS_WHLD) },
    [1] = { ENCODER_CCW_CW(MS_WHLU, MS_WHLD) },
    [2] = { ENCODER_CCW_CW(MS_WHLU, MS_WHLD) },
    [3] = { ENCODER_CCW_CW(MS_WHLU, MS_WHLD) }
};

#endif