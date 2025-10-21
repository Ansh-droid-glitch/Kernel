#include "../../intf/print.h"
#include "../../intf/idt.h"
#include "../../intf/fs.h"
#include "../../intf/keyboard.h"
#include <stdint.h>

#define INPUT_BUFFER_SIZE 128

extern volatile uint8_t key_buffer[16];
extern volatile size_t buffer_head;
extern volatile size_t buffer_tail;
extern void pic_init(void);

static int shift = 0;
volatile uint8_t key_states[128] = {0};

void buffer_add(uint8_t scancode) {
    size_t next_head = (buffer_head + 1) % 16;
    if (next_head != buffer_tail) {
        key_buffer[buffer_head] = scancode;
        buffer_head = next_head;
    }
}

int buffer_get(uint8_t *scancode) {
    if (buffer_head == buffer_tail) return 0;
    *scancode = key_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % 16;
    return 1;
}

char scancode_to_ascii(uint8_t scancode) {
    static const char map[128] = {
        0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
        'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
        'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' ', 0
    };
    if (scancode == 0x2A || scancode == 0x36) { shift = 1; return 0; }
    if (scancode == (0x2A | 0x80) || scancode == (0x36 | 0x80)) { shift = 0; return 0; }
    if (scancode & 0x80) return 0;
    char c = map[scancode];
    if (shift && c >= 'a' && c <= 'z') c -= 32;
    return c;
}

int str_equals(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return (*a == *b);
}

static uint32_t seed = 123456789;

uint32_t rand32(void) {
    seed = seed * 1664525 + 1013904223;
    return seed;
}

uint32_t rand_range(uint32_t min, uint32_t max) {
    return (rand32() % (max - min + 1)) + min;
}

int char_to_int(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else
        return -1;
}

int str_to_int(const char *str) {
    int result = 0;
    while (*str) {
        char c = *str;
        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
        } else {
            break;
        }
        str++;
    }
    return result;
}

void keyboard_poll_for_game(uint32_t randum) {
    static char input_buffer[INPUT_BUFFER_SIZE];
    static size_t input_len = 0;
    uint8_t scancode;
    if (buffer_get(&scancode)) {
        char c = scancode_to_ascii(scancode);
        if (c) {
            if (c == '\n' || scancode == 28) {
                input_buffer[input_len] = '\0';
                print_str("\n> ");
                int guess = str_to_int(input_buffer);
                if(randum == guess){
                    print_str("You got it right!\n");
                    input_len = 0;
                    return;
                } else {
                    print_str("Wrong! Try again: ");
                }
                input_len = 0;
            } 
            else if (input_len < INPUT_BUFFER_SIZE - 1) {
                input_buffer[input_len++] = c;
                char str[2] = {c, '\0'};
                print_str(str);
            }
        }
    }
}
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 100000; i++) {
        __asm__ volatile ("nop");
    }
}
void game() {
    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Welcome to Random Guessing Number Game \n");
    uint32_t randnum = rand_range(1, 10);
    print_str("Guess a number from 1 to 10: \n");
    while(1){
        keyboard_poll_for_game(randnum);
        delay_ms(100);
    }
}
int starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return 1;
}

void handle_print_command(const char *input) {
    const char *text = input + 6; // skip "print "
    if (*text == '\0') return; // nothing to print
    print_str(text);
    print_str("\n> ");
}

static void ls_print_cb(const char *name, fs_node_type_t type, void *user) {
    (void)user;
    if (type == FS_NODE_DIR) {
        print_str("[DIR] ");
    } else {
        print_str("[FILE] ");
    }
    print_str(name);
    print_str("\n");
}

static void cmd_ls(const char *arg) {
    const char *path = (*arg) ? arg : "/";
    if (fs_ls(path, ls_print_cb, 0) < 0) {
        print_str("ls: not found\n");
    }
}

static void cmd_mkdir(const char *path) {
    if (!*path) { print_str("mkdir: path required\n"); return; }
    if (fs_mkdir(path) < 0) {
        print_str("mkdir: failed\n");
    }
}

static void cmd_touch(const char *path) {
    if (!*path) { print_str("touch: path required\n"); return; }
    if (fs_touch(path) < 0) {
        print_str("touch: failed\n");
    }
}

static void cmd_cat(const char *path) {
    if (!*path) { print_str("cat: path required\n"); return; }
    char buf[512];
    int n = fs_read(path, buf, sizeof(buf) - 1);
    if (n < 0) { print_str("cat: failed\n"); return; }
    buf[n] = '\0';
    print_str(buf);
    print_str("\n");
}

static void cmd_write(const char *args) {
    // args: <path> <text>
    // find first space
    const char *p = args;
    while (*p && *p != ' ') p++;
    if (!*p) { print_str("write: usage write <path> <text>\n"); return; }
    // path is args[0..p)
    char path[64];
    size_t i = 0;
    for (const char *q = args; q < p && i + 1 < sizeof(path); q++, i++) path[i] = *q;
    path[i] = '\0';
    // skip spaces
    while (*p == ' ') p++;
    const char *text = p;
    size_t len = 0; while (text[len]) len++;
    if (fs_write(path, text, len) < 0) {
        print_str("write: failed\n");
    }
}

void keyboard_poll(void) {
    static char input_buffer[INPUT_BUFFER_SIZE];
    static size_t input_len = 0;
    uint8_t scancode;

    if (buffer_get(&scancode)) {
        char c = scancode_to_ascii(scancode);

        if (c) {
            if (c == '\n' || scancode == 28) { // Enter
                input_buffer[input_len] = '\0';
                print_str("\n> ");
                if (str_equals(input_buffer, "help")) {
                    print_str("game - starts the game menu\n");
                    print_str("print <text> - prints the text\n");
                    print_str("ls [path] - list directory or file\n");
                    print_str("mkdir <path> - create directory\n");
                    print_str("touch <path> - create empty file\n");
                    print_str("write <path> <text> - write file\n");
                    print_str("cat <path> - print file contents\n>");
                } else if (str_equals(input_buffer, "game")) {
                    game();
                } else if (starts_with(input_buffer, "print ")) {
                    handle_print_command(input_buffer);
                } else if (starts_with(input_buffer, "ls")) {
                    const char *arg = input_buffer + 2;
                    while (*arg == ' ') arg++;
                    cmd_ls(arg);
                } else if (starts_with(input_buffer, "mkdir ")) {
                    cmd_mkdir(input_buffer + 6);
                } else if (starts_with(input_buffer, "touch ")) {
                    cmd_touch(input_buffer + 6);
                } else if (starts_with(input_buffer, "cat ")) {
                    cmd_cat(input_buffer + 4);
                } else if (starts_with(input_buffer, "write ")) {
                    fs_write(input_buffer + 6);
                }
                input_len = 0;
            } 
            else if (scancode == 0x0E) { // Backspace
                if (input_len > 0) {
                input_len--;
                print_backspace(); // <-- clean!
                }
            }

            else if (input_len < INPUT_BUFFER_SIZE - 1) { // Normal character
                input_buffer[input_len++] = c;
                char str[2] = {c, '\0'};
                print_str(str);
            }
        }
    }
}





void kernel_main() {
    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Welcome to Beacon OS 64-bit\n");
    print_set_color(PRINT_COLOR_CYAN, PRINT_COLOR_BLACK);
    print_str("Type help for a list of commands");
    fs_init();
    // Seed FS with a README
    fs_write("/README.txt", "Welcome to BeaconOS FS. Try: ls /, cat /README.txt", 54);
    idt_init();
    pic_init();
    __asm__ volatile ("sti");
    print_str("\n> ");
    while (1) {
        keyboard_poll();
        delay_ms(100);
    }
}
