#include "quantum.h"
#include "via.h"
#include "vial.h"

#define LUO_PROTOCOL_VERSION 0x01

#define LUO_PREFIX 0xfd

#define LUO_CMD_GET_VERSION 0x00
#define LUO_CMD_GET_STATE 0x01


void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if(data[0] != LUO_PREFIX){
        return;
    }

    switch (data[1]) {
        case LUO_CMD_GET_VERSION:
            uint8_t version = LUO_PROTOCOL_VERSION;
            data[0] = version & 0xFF;
            break;

        case LUO_CMD_GET_STATE:
            // memory layout
            // 0-1: layer_state
            // 2-12: matrix state
            // 13: mods
            // 14: oneshot_mods

            uint16_t state = layer_state | default_layer_state;
            data[0] = (state >> 8) & 0xFF;
            data[1] = state & 0xFF;
            data[13] = get_mods() & 0xFF;
            data[14] = get_oneshot_mods() & 0xFF;

            // modified quantum/via.c:250
            if (!vial_unlocked)
                break;
            uint8_t i = 2;
            for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                matrix_row_t value = matrix_get_row(row);
                data[i++] = value & 0xFF;
            }

            break;
    }
}
