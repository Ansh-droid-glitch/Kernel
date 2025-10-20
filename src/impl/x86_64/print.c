#include "print.h"

const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;

struct Char {
    uint8_t character;
    uint8_t color;
};

struct Char* buffer = (struct Char*) 0xb8000;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | (PRINT_COLOR_BLACK << 4);

void clear_row(size_t row) {
    struct Char empty = {
        .character = ' ',
        .color = color,
    };
    for (size_t c = 0; c < NUM_COLS; c++) {
        buffer[c + NUM_COLS * row] = empty;
    }
}

void print_clear() {
    for (size_t r = 0; r < NUM_ROWS; r++) {
        clear_row(r);
    }
    col = 0;
    row = 0;
}

void print_newline() {
    col = 0;
    if (row < NUM_ROWS - 1) {
        row++;
        return;
    }

    for (size_t r = 1; r < NUM_ROWS; r++) {
        for (size_t c = 0; c < NUM_COLS; c++) {
            buffer[c + NUM_COLS * (r - 1)] = buffer[c + NUM_COLS * r];
        }
    }

    clear_row(NUM_ROWS - 1);
}

void print_char(char character) {
    if (character == '\n') {
        print_newline();
        return;
    }

    if (col >= NUM_COLS) {
        print_newline();
    }

    buffer[col + NUM_COLS * row] = (struct Char){
        .character = (uint8_t) character,
        .color = color,
    };

    col++;
}

void print_str(const char* str) {
    for (size_t i = 0; ; i++) {
        char c = str[i];
        if (c == '\0') return;
        print_char(c);
    }
}

void print_set_color(uint8_t foreground, uint8_t background) {
    color = foreground | (background << 4);
}

// -------------------- NEW --------------------
void print_backspace() {
    if (col == 0 && row == 0) return; // nothing to delete

    if (col == 0) {
        row--;
        col = NUM_COLS - 1;
    } else {
        col--;
    }

    buffer[col + NUM_COLS * row].character = ' ';
    buffer[col + NUM_COLS * row].color = color;
}
