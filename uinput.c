/*
    This file is part of ydotool.
	Copyright (C) 2019 Harry Austen
    Copyright (C) 2018-2019 ReimuNotMoe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/* Local includes */
#include "uinput.h"
/* System includes */
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <sys/stat.h>

/* Wrapper macro for errno error check */
#define CHECK(X) if (X == -1) { fprintf( stderr, "ERROR (%s:%d) -- %s\n", __FILE__, __LINE__, strerror(errno) ); exit(-1); }

/* uinput file descriptor */
int FD = -1;

/* All valid keycodes */
const int KEYCODES[] = {
    BTN_LEFT, BTN_RIGHT, BTN_MIDDLE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
    KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_Q, KEY_W,
    KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFTBRACE,
    KEY_RIGHTBRACE, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J,
    KEY_K, KEY_L, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_GRAVE, KEY_LEFTSHIFT,
    KEY_BACKSLASH, KEY_102ND, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N,
    KEY_M, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_SPACE, KEY_TAB, KEY_ENTER
};

/* All valid event codes */
int EVCODES[] = {
    EV_KEY,
    EV_REL,
    EV_ABS,
    EV_SYN
};

/* All valid non-shifted characters */
const char NORMAL_KEYS[] = {
    '\t', '\n', ' ', '#', '\'', ',', '-', '.', '/', '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9', ';', '=', '[', '\\', ']', '`', 'a',
    'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

/* All valid non-shifted keycodes */
const int NORMAL_KEYCODES[] = {
    KEY_TAB, KEY_ENTER, KEY_SPACE, KEY_BACKSLASH, KEY_APOSTROPHE, KEY_COMMA, KEY_MINUS,
    KEY_DOT, KEY_SLASH, KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
    KEY_6, KEY_7, KEY_8, KEY_9, KEY_SEMICOLON, KEY_EQUAL,
    KEY_LEFTBRACE, KEY_102ND, KEY_RIGHTBRACE, KEY_GRAVE, KEY_A,
    KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S,
    KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z
};

/* All valid shifted characters */
const char SHIFTED_KEYS[] = {
    '!', '"', '$', '%', '&', '(', ')', '*', '+', ':', '<', '>',
    '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', '^', '_', '{', '|', '}', '~'
};

/* All valid shifted keycodes */
const int SHIFTED_KEYCODES[] = {
    KEY_1, KEY_2, KEY_4, KEY_5, KEY_7, KEY_9, KEY_0, KEY_8, KEY_EQUAL,
    KEY_SEMICOLON, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_APOSTROPHE,
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I,
    KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
    KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z, KEY_6,
    KEY_MINUS, KEY_LEFTBRACE, KEY_102ND, KEY_RIGHTBRACE, KEY_BACKSLASH
};

struct key {
    char string[11];
    int code;
};

const struct key MODIFIER_KEYS[] = {
	{"ALT", KEY_LEFTALT}, {"ALT_L", KEY_LEFTALT}, {"ALT_R", KEY_RIGHTALT},
	{"SHIFT", KEY_LEFTSHIFT}, {"SHIFT_L", KEY_LEFTSHIFT}, {"SHIFT_R", KEY_RIGHTSHIFT},
	{"CTRL", KEY_LEFTCTRL}, {"CTRL_L", KEY_LEFTCTRL}, {"CTRL_R", KEY_RIGHTCTRL},
	{"SUPER", KEY_LEFTMETA}, {"SUPER_L", KEY_LEFTMETA}, {"SUPER_R", KEY_RIGHTMETA},
	{"META", KEY_LEFTMETA}, {"META_L", KEY_LEFTMETA}, {"META_R", KEY_RIGHTMETA}
};

const struct key FUNCTION_KEYS[] = {
	{"UP", KEY_UP}, {"DOWN", KEY_DOWN}, {"LEFT", KEY_LEFT}, {"RIGHT", KEY_RIGHT},
	{"TAB", KEY_TAB}, {"CAPSLOCK", KEY_CAPSLOCK}, {"NUMLOCK", KEY_NUMLOCK}, {"SCROLLLOCK", KEY_SCROLLLOCK}, {"ESC", KEY_ESC}, {"ENTER", KEY_ENTER}, {"BACKSPACE", KEY_BACKSPACE}, {"DELETE", KEY_DELETE},
	{"INSERT", KEY_INSERT}, {"HOME", KEY_HOME}, {"END", KEY_END}, {"PAGEUP", KEY_PAGEUP}, {"PAGEDOWN", KEY_PAGEDOWN}, {"SYSRQ", KEY_SYSRQ}, {"PAUSE", KEY_PAUSE},
	{"F1", KEY_F1}, {"F2", KEY_F2}, {"F3", KEY_F3}, {"F4", KEY_F4}, {"F5", KEY_F5}, {"F6", KEY_F6}, {"F7", KEY_F7}, {"F8", KEY_F8}, {"F9", KEY_F9}, {"F10", KEY_F10}, {"F11", KEY_F11}, {"F12", KEY_F12}
};

/* Safely exit program with error code */
void uinput_error() {
    uinput_destroy();
    exit(-1);
}

/* Initialise the input device */
void uinput_init() {
    /* Check write access to uinput driver device */
    if (access("/dev/uinput", W_OK)) {
        fprintf(stderr, "Do not have access to write to /dev/uinput!\nTry running as root\n");
        uinput_error();
    }

    /* Confirm availability of uinput kernel module */
    struct utsname uname_buffer;
    CHECK( uname(&uname_buffer) );
    char kernel_mod_dir[50] = "/lib/modules/";
    strcat(kernel_mod_dir, uname_buffer.release);
    struct stat stats;
    stat(kernel_mod_dir, &stats);
    if (!S_ISDIR(stats.st_mode)) {
        fprintf(stderr, "Dir (%s) doesn't exist!\nHave you recently updated your kernel version?\nRestart your system to use new kernel modules\n", kernel_mod_dir);
        uinput_error();
    }

    /* Open uinput driver device */
    CHECK( (FD = open("/dev/uinput", O_WRONLY|O_NONBLOCK)) );

    /* Events/Keys setup */
    for (int i = 0; i != sizeof(KEYCODES)/sizeof(KEYCODES[0]); ++i) {
        CHECK( ioctl(FD, UI_SET_KEYBIT, KEYCODES[i]) );
    }
    for (int i = 0; i != sizeof(EVCODES)/sizeof(EVCODES[0]); ++i) {
        CHECK( ioctl(FD, UI_SET_EVBIT, EVCODES[i]) );
    }

    /* uinput device setup */
    struct uinput_setup usetup = {
        {
            BUS_USB,
            0x1234,
            0x5678,
            0
        },
        "ydotool virtual device",
        0
    };

    CHECK( ioctl(FD, UI_DEV_SETUP, &usetup) );
    CHECK( ioctl(FD, UI_DEV_CREATE) );
}

/* Delete the input device */
void uinput_destroy() {
    if (FD != -1) {
        CHECK( ioctl(FD, UI_DEV_DESTROY) );
        close(FD);
        FD = -1;
    }
}

/* Character pointer comparer for use with bsearch */
int cmp_chars(const void * a, const void * b) {
    return (*(char *)a - *(char *)b);
}

/* TODO: Implement binary search */
void uinput_enter_key(const char * key_string) {
    for (int i = 0; i != sizeof(MODIFIER_KEYS)/sizeof(struct key); ++i) {
        if (!strcmp(key_string, MODIFIER_KEYS[i].string)) {
            uinput_send_keypress(MODIFIER_KEYS[i].code);
            return;
        }
    }
    for (int i = 0; i != sizeof(FUNCTION_KEYS)/sizeof(struct key); ++i) {
        if (!strcmp(key_string, FUNCTION_KEYS[i].string)) {
            uinput_send_keypress(FUNCTION_KEYS[i].code);
            return;
        }
    }
    /* TODO: Check upper/lower keys too */
}

/* Simulate typing the given character on the vitual device */
void uinput_enter_char(char c) {
    void * found;
    void * begin_norm = (void *)&NORMAL_KEYS;
    void * begin_shift = (void *)&SHIFTED_KEYS;

    if ((found = bsearch(&c, begin_norm, sizeof(NORMAL_KEYS)/sizeof(char), sizeof(char), cmp_chars))) {
        uinput_send_keypress(NORMAL_KEYCODES[((char *)found - (char *)begin_norm)/sizeof(char)]);
    } else if ((found = bsearch(&c, begin_shift, sizeof(SHIFTED_KEYS)/sizeof(char), sizeof(char), cmp_chars))) {
        uinput_send_shifted_keypress(SHIFTED_KEYCODES[((char *)found - (char *)begin_shift)/sizeof(char)]);
    } else {
        fprintf(stderr, "Unsupported character (%d:%c) cannot be entered!\n", c, c);
    }
}

/* Trigger an input event */
void uinput_emit(uint16_t type, uint16_t code, int32_t value) {
    struct input_event ie = {
        /* Ignore timestamp values */
        {0,0},
        type,
        code,
        value
    };

    if (FD == -1) {
        uinput_init();
    }

    CHECK( write(FD, &ie, sizeof(ie)) );

    /* Allow processing time for uinput before sending next event */
    usleep( 50 );
}

/* Single key event and report */
void uinput_send_key(uint16_t code, int32_t value) {
    uinput_emit(EV_KEY, code,       value);
    uinput_emit(EV_SYN, SYN_REPORT, 0    );
}

/* Simulate a quick key press */
void uinput_send_keypress(uint16_t code) {
    /* send press */
    uinput_send_key(code, 1);
    /* send release */
    uinput_send_key(code, 0);
}

/* Simulate a shifted key press */
void uinput_send_shifted_keypress(uint16_t code) {
    /* Send shift press */
    uinput_send_key(KEY_LEFTSHIFT, 1);
    /* Simulate keypress */
    uinput_send_keypress(code);
    /* Send shift release */
    uinput_send_key(KEY_LEFTSHIFT, 0);
}

/* Move the cursor to a given (x,y) position */
void uinput_move_mouse(int32_t x, int32_t y) {
    uinput_emit(EV_ABS, ABS_X, x);
    uinput_emit(EV_ABS, ABS_Y, y);
    uinput_emit(EV_SYN, SYN_REPORT, 0);
}

/* Move the cursor a given (x,y) relative to the current position */
void uinput_relative_move_mouse(int32_t x, int32_t y) {
    if (x) {
        uinput_emit(EV_REL, REL_X, x);
    }
    if (y) {
        uinput_emit(EV_REL, REL_Y, y);
    }
    uinput_emit(EV_SYN, SYN_REPORT, 0);
}
