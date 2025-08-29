#include <stdint.h>
#include <stdbool.h>

#include "config.h"

#include "quantum.h"
#include "action.h"
#include "gpio.h"
#include "keyboard.h"
#include "matrix.h"

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
static const uint8_t col_pushed_states[MATRIX_COLS] = MATRIX_COL_PUSHED_STATES;

static inline void setPinOutput_writeHigh(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinOutput(pin);
        writePinHigh(pin);
    }
}

static inline void setPinOutput_writeLow(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinOutput(pin);
        writePinLow(pin);
    }
}

void matrix_init_custom(void) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        setPinOutput_writeHigh(row_pins[row]);
    }

    for (int col = 0; col < MATRIX_COLS; col++) {
        setPinInput(col_pins[col]);
    }
}

matrix_row_t read_row(void) {
    matrix_row_t row = 0;

    for (int col = 0; col < MATRIX_COLS; col++) {
        int col_pin = col_pins[col];
        uint8_t pin_state =readPin(col_pin);

        if (pin_state == col_pushed_states[col]) {
            row |= 1 << col;
        }
    }

    return row;
}


bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;
    for (int row = 0; row < MATRIX_ROWS; row++) {
        pin_t row_pin = row_pins[row];
        setPinOutput_writeLow(row_pin);
        wait_us(15);

        matrix_row_t new_row = read_row();
        changed |= new_row != current_matrix[row];
        current_matrix[row] = new_row;
        setPinOutput_writeHigh(row_pin);

        wait_us(100);
    }
    return changed;
}

void matrix_power_down(void) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        setPinOutput_writeHigh(row_pins[row]);
    }
}
