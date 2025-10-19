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
    struct Char empty = (struct Char) {
        .character = ' ',   // ✅ fixed (single char + proper designated initializer)
        .color = color,
    };

    // ✅ fixed loop syntax: use semicolon, not comma
    for (size_t col = 0; col < NUM_COLS; col++) {
        buffer[col + NUM_COLS * row] = empty;
    }
}

void print_clear() {
    for (size_t i = 0; i < NUM_ROWS; i++) {
        clear_row(i);
    }
}

void print_newline() {
    col = 0;
    if (row < NUM_ROWS - 1) {
        row++;
        return;
    }

    for (size_t r = 1; r < NUM_ROWS; r++) {
        for (size_t c = 0; c < NUM_COLS; c++) {
            struct Char character = buffer[c + NUM_COLS * r];
            buffer[c + NUM_COLS * (r - 1)] = character;
        }
    }

    // ✅ fix: was NUM_COLS - 1 (wrong), should clear last row
    clear_row(NUM_ROWS - 1);
}

void print_char(char character) {
    if (character == '\n') {
        print_newline();
        return;
    }

    // ✅ fix: should wrap when col == NUM_COLS
    if (col >= NUM_COLS) {
        print_newline();
    }

    buffer[col + NUM_COLS * row] = (struct Char) {
        .character = (uint8_t) character,
        .color = color,
    };

    col++;
}

void print_str(const char* str) {
    for (size_t i = 0; ; i++) {
        char character = str[i];
        if (character == '\0') {
            return;
        }
        print_char(character);
    }
}

void print_set_color(uint8_t foreground, uint8_t background) {
    color = foreground | (background << 4);  // ✅ fix: use OR instead of +
}
