#include <string.h>

#include "raw_hid.h"
#include "quantum.h"
#include "eeconfig.h"
#include "via.h"
#include "vial.h"
#include "hardware/flash.h"
#include "matrix.h"
#include "timer.h"

#define LUO_PROTOCOL_VERSION 0x01

#define LUO_PREFIX 0xfd

#define LUO_CMD_GET_VERSION 0x01
#define LUO_CMD_GET_FLAG 0x02
#define LUO_CMD_SET_FLAG 0x03
#define LUO_CMD_GET_LAYER_NAME 0x04
#define LUO_CMD_SET_LAYER_NAME 0x05
#define LUO_CMD_GET_KEYMAP_NAME 0x06
#define LUO_CMD_SET_KEYMAP_NAME 0x07
#define LUO_CMD_BROADCAST 0x08

#define LUO_CONFIG_INIT_MAGIC 20250810
#define LUO_LAYER_COUNT 16
#define LUO_PRACTICE_LAYER 15
#define LUO_LAYER_NAME_SIZE 28
#define LUO_KEYMAP_NAME_SIZE 28

struct luo_config_t {
    uint32_t init_magic;
    char layer_names[LUO_LAYER_COUNT][LUO_LAYER_NAME_SIZE];
    char keymap_name[LUO_KEYMAP_NAME_SIZE];
};

struct luo_config_t luo_config;
uint8_t broadcast_flag = 0;
uint8_t practice_flag = 0;

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
            uint8_t version = LUO_PROTOCOL_VERSION;
            data[2] = version;
            flash_get_unique_id(&data[3]);
            break;

        case LUO_CMD_GET_FLAG:
            data[2] = broadcast_flag;
            data[3] = practice_flag;
            break;

        case LUO_CMD_SET_FLAG:
            broadcast_flag = data[2];
            practice_flag = data[3];
            if(practice_flag){
                layer_on(LUO_PRACTICE_LAYER);
            }else{
                layer_off(LUO_PRACTICE_LAYER);
            }
            break;

        case LUO_CMD_GET_LAYER_NAME:
            uint8_t layer_index = data[2];
            memcpy(&data[3],&luo_config.layer_names[layer_index],LUO_LAYER_NAME_SIZE);
            break;

        case LUO_CMD_SET_LAYER_NAME:
            layer_index = data[2];
            memcpy(&luo_config.layer_names[layer_index],&data[3],LUO_LAYER_NAME_SIZE);
            eeconfig_update_kb_datablock(&luo_config);
            break;

        case LUO_CMD_GET_KEYMAP_NAME:
            memcpy(&data[2],&luo_config.keymap_name,LUO_KEYMAP_NAME_SIZE);
            break;

        case LUO_CMD_SET_KEYMAP_NAME:
            memcpy(&luo_config.keymap_name,&data[2],LUO_KEYMAP_NAME_SIZE);
            eeconfig_update_kb_datablock(&luo_config);
            break;
    }
}

static uint16_t broadcast_timer;
#define BROADCAST_TIMEOUT 16

static uint8_t broadcast_data_old[32];
static uint8_t broadcast_data[32];

static void broadcast(void) {
    // start:   modified quantum/via.c:250
    if (!vial_unlocked)
        return;

    uint8_t i = 2;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        matrix_row_t value = matrix_get_row(row);
        broadcast_data[i++] = value & 0xFF;
    }
    // end

    broadcast_data[0] = LUO_PREFIX & 0xFF;
    broadcast_data[1] = LUO_CMD_BROADCAST & 0xFF;

    uint16_t state = layer_state | default_layer_state;
    broadcast_data[13] = (state >> 8) & 0xFF;
    broadcast_data[14] = state & 0xFF;
    broadcast_data[15] = get_mods() & 0xFF;
    broadcast_data[16] = get_oneshot_mods() & 0xFF;

    if(memcmp(broadcast_data_old,broadcast_data,32)==0) {
        return;
    }
    memcpy(broadcast_data_old,broadcast_data,32);
    raw_hid_send(broadcast_data,32);
}

void housekeeping_task_kb(void) {
    if(!broadcast_flag) {
        return;
    }

    if(timer_elapsed(broadcast_timer) > BROADCAST_TIMEOUT) {
        broadcast();
        broadcast_timer = timer_read();
    }
}
