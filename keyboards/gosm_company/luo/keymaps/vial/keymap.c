#include "quantum_keycodes.h"
#include QMK_KEYBOARD_H

enum layer {
    NORMAL,
};

const uint16_t PROGMEM keymaps[DYNAMIC_KEYMAP_LAYER_COUNT][MATRIX_ROWS][MATRIX_COLS] = {
    [NORMAL] = LAYOUT(
        KC_LBRC,    KC_A,       KC_Z,       KC_Q,       KC_DEL,     KC_1,
        KC_B,       KC_S,       KC_X,       KC_W,       KC_ESC,     KC_2,
        KC_T,       KC_D,       KC_C,       KC_E,       KC_GRV,     KC_3,
        KC_G,       KC_F,       KC_V,       KC_R,       KC_QUOT,    KC_4,
        KC_NO,      KC_9,       KC_LCTL,    KC_TAB,     KC_SPC,     KC_0,

        XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,
        XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,
        XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,
        XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,
        XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX
    ),
    
};

void keyboard_post_init_user(void) {
  debug_enable=true;
  debug_matrix=true;
};
