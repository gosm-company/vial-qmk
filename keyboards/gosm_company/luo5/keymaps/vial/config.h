#pragma once

#define VIAL_KEYBOARD_UID {0xCB, 0x3C, 0x80, 0x65, 0x51, 0x84, 0x9A, 0x87}

// DO NOT CHANGE THIS VALUE
#define DYNAMIC_KEYMAP_LAYER_COUNT 16

// Undocumented QMK feature
// virtual eeconfig_init_kb_datablock()
// eeconfig_read_kb_datablock(void*)
// eeconfig_update_kb_datablock(void*)
//
#define EECONFIG_KB_DATA_SIZE 256
