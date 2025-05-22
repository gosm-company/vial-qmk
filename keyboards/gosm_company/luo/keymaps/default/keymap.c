#include QMK_KEYBOARD_H

enum layer {
   NORMAL
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [NORMAL] = LAYOUT(
        KC_LBRC,    KC_A,       KC_Z,       KC_Q,       KC_DEL,     KC_1,
        KC_B,       KC_S,       KC_X,       KC_W,       KC_ESC,     KC_2,
        KC_T,       KC_D,       KC_C,       KC_E,       KC_GRV,     KC_3,
        KC_G,       KC_F,       KC_V,       KC_R,       KC_QUOT,    KC_4,
        KC_NO,      KC_9,       KC_LCTL,    KC_TAB,     KC_SPC,     KC_0,

        KC_NO,      KC_DOWN,    KC_HOME,    KC_ENT,     KC_BSPC,    KC_RALT,
        KC_MINS,    KC_J,       KC_M,       KC_U,       KC_H,       KC_5,
        KC_EQL,     KC_K,       KC_COMM,    KC_I,       KC_Y,       KC_6,
        KC_END,     KC_L,       KC_DOT,     KC_O,       KC_N,       KC_7,
        KC_BSLS,    KC_SCLN,    KC_SLSH,    KC_P,       KC_RBRC,    KC_8
    ),
};

void keyboard_post_init_user(void) {
  debug_enable=true;
  debug_matrix=true;
};
