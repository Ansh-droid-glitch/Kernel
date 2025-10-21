#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stddef.h>

// External keyboard buffer variables
extern volatile uint8_t key_buffer[16];
extern volatile size_t buffer_head;
extern volatile size_t buffer_tail;

// External PIC initialization function
extern void pic_init(void);

// Keyboard functions
void buffer_add(uint8_t scancode);
int buffer_get(uint8_t *scancode);
char scancode_to_ascii(uint8_t scancode);
void keyboard_poll(void);
void keyboard_poll_for_game(uint32_t randum, int *exit_game);


#endif
