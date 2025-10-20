#ifndef PRINT_H
#define PRINT_H

#include <stddef.h>
#include <stdint.h>

#define PRINT_COLOR_BLACK 0
#define PRINT_COLOR_BLUE 1
#define PRINT_COLOR_GREEN 2
#define PRINT_COLOR_CYAN 3
#define PRINT_COLOR_RED 4
#define PRINT_COLOR_MAGENTA 5
#define PRINT_COLOR_BROWN 6
#define PRINT_COLOR_LIGHT_GREY 7
#define PRINT_COLOR_DARK_GREY 8
#define PRINT_COLOR_LIGHT_BLUE 9
#define PRINT_COLOR_LIGHT_GREEN 10
#define PRINT_COLOR_LIGHT_CYAN 11
#define PRINT_COLOR_LIGHT_RED 12
#define PRINT_COLOR_LIGHT_MAGENTA 13
#define PRINT_COLOR_LIGHT_BROWN 14
#define PRINT_COLOR_WHITE 15
#define PRINT_COLOR_YELLOW 14

void print_clear();
void print_char(char character);
void print_str(const char* string);
void print_set_color(uint8_t foreground, uint8_t background);
void print_backspace();  // ← new function

#endif
