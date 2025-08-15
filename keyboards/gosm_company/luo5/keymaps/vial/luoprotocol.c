#include <string.h>

#include "quantum.h"
#include "eeconfig.h"
#include "via.h"
#include "vial.h"
#include "hardware/flash.h"

#define LUO_PROTOCOL_VERSION 0x01

#define LUO_PREFIX 0xfd

#define LUO_CMD_GET_VERSION 0x00
#define LUO_CMD_GET_STATE 0x01
#define LUO_CMD_GET_LAYER_NAME 0x02
#define LUO_CMD_SET_LAYER_NAME 0x03
#define LUO_CMD_GET_KEYMAP_NAME 0x04
#define LUO_CMD_SET_KEYMAP_NAME 0x05

#define LUO_CONFIG_INIT_MAGIC 123456789
#define LUO_LAYER_COUNT 16
#define LUO_LAYER_NAME_SIZE 28
#define LUO_KEYMAP_NAME_SIZE 28

struct luo_config_t {
    uint32_t init_magic;
    char layer_names[LUO_LAYER_COUNT][LUO_LAYER_NAME_SIZE];
    char keymap_name[LUO_KEYMAP_NAME_SIZE];
};

struct luo_config_t luo_config;

void keyboard_post_init_kb() {
    eeconfig_read_kb_datablock(&luo_config);
    if(luo_config.init_magic!=LUO_CONFIG_INIT_MAGIC){
        luo_config.init_magic=LUO_CONFIG_INIT_MAGIC;
        for(int i=0;i<LUO_LAYER_COUNT;i+=1){
            memset(luo_config.layer_names[i],0,LUO_LAYER_NAME_SIZE);
            strcpy(luo_config.layer_names[i],"noname");
        }

        memset(luo_config.keymap_name,0,LUO_KEYMAP_NAME_SIZE);
        strcpy(luo_config.keymap_name,"default");
    }
    eeconfig_update_kb_datablock(&luo_config);
}

bool shutdown_kb(bool jump_to_bootloader) {
    luo_config.init_magic = 0;
    eeconfig_update_kb_datablock(&luo_config);
    return true;
}

void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if(data[0] != LUO_PREFIX){
        return;
    }

    switch (data[1]) {
        case LUO_CMD_GET_VERSION:
            // memory layout
            // 0:       protocol version
            // 1-8:     hardware_id

            uint8_t version = LUO_PROTOCOL_VERSION;
            data[0] = version & 0xFF;

            flash_get_unique_id(&data[1]);
            break;

        case LUO_CMD_GET_STATE:
            // memory layout
            // 0-1:     layer_state
            // 2-12:    matrix state
            // 13:      mods
            // 14:      oneshot_mods

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

        case LUO_CMD_GET_LAYER_NAME:
            // memory layout
            // 0:       layer index
            // 1-29:    layer name

            uint8_t layer_index = data[2];
            data[0]=layer_index;
            memcpy(&data[1],&luo_config.layer_names[layer_index],LUO_LAYER_NAME_SIZE);
            break;

        case LUO_CMD_SET_LAYER_NAME:
            layer_index = data[2];
            memcpy(&luo_config.layer_names[layer_index],&data[3],LUO_LAYER_NAME_SIZE);
            eeconfig_update_kb_datablock(&luo_config);
            break;

        case LUO_CMD_GET_KEYMAP_NAME:
            // memory layout
            // 0-27:    keymap_name
            memcpy(&data[0],&luo_config.keymap_name,LUO_KEYMAP_NAME_SIZE);
            break;

        case LUO_CMD_SET_KEYMAP_NAME:
            memcpy(&luo_config.keymap_name,&data[2],LUO_KEYMAP_NAME_SIZE);
            eeconfig_update_kb_datablock(&luo_config);
            break;
    }
}
