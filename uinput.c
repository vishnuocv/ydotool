/// @copyright
/// This file is part of ydotool.
/// Copyright (C) 2019 Harry Austen
/// Copyright (C) 2018-2019 ReimuNotMoe
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the MIT License.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

/// @file uinput.c
/// @author Harry Austen
/// @brief Implementation of functions for emulating input events
/// @todo Implement time delay inputs

// System includes
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

// Local includes
#include "uinput.h"

/// Wrapper macro for errno error check
#define CHECK(X) if (X == -1) { fprintf( stderr, "ERROR (%s:%d) -- %s\n", __FILE__, __LINE__, strerror(errno) ); return 1; }

/// Total number of keycodes that can be entered
#define NUM_KEYCODES 91

/// Total number of event codes that can be sent
#define NUM_EVCODES 4

struct uinput_user_dev uidev;

/// uinput file descriptor
static int FD = -1;

/// All valid keycodes
static const int KEYCODES[NUM_KEYCODES] = {
    BTN_LEFT, BTN_RIGHT, BTN_MIDDLE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
    KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_Q, KEY_W,
    KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFTBRACE,
    KEY_RIGHTBRACE, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J,
    KEY_K, KEY_L, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_GRAVE,
    KEY_BACKSLASH, KEY_102ND, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N,
    KEY_M, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_SPACE, KEY_TAB, KEY_ENTER,
	KEY_LEFTALT, KEY_RIGHTALT, KEY_LEFTSHIFT, KEY_RIGHTSHIFT, KEY_LEFTCTRL,
    KEY_RIGHTCTRL, KEY_LEFTMETA, KEY_RIGHTMETA, KEY_UP, KEY_DOWN, KEY_LEFT,
    KEY_RIGHT, KEY_CAPSLOCK, KEY_NUMLOCK, KEY_SCROLLLOCK, KEY_ESC,
    KEY_BACKSPACE, KEY_DELETE, KEY_INSERT, KEY_HOME, KEY_END,
    KEY_PAGEUP, KEY_PAGEDOWN, KEY_SYSRQ, KEY_PAUSE, KEY_F1, KEY_F2, KEY_F3,
    KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,  KEY_F11, KEY_F12
};

/// All valid event codes
static const int EVCODES[NUM_EVCODES] = {
    EV_KEY,
    EV_REL,
    EV_ABS,
    EV_SYN
};

/// All valid non-shifted characters
/// Used for mapping the char representation to the uinput keycode
const struct key_char NORMAL_KEYS[] = {
    {'\t', KEY_TAB},
    {'\n', KEY_ENTER},
    {' ', KEY_SPACE},
    {'#', KEY_BACKSLASH},
    {'\'', KEY_APOSTROPHE},
    {',', KEY_COMMA},
    {'-', KEY_MINUS},
    {'.', KEY_DOT},
    {'/', KEY_SLASH},
    {'0', KEY_0},
    {'1', KEY_1},
    {'2', KEY_2},
    {'3', KEY_3},
    {'4', KEY_4},
    {'5', KEY_5},
    {'6', KEY_6},
    {'7', KEY_7},
    {'8', KEY_8},
    {'9', KEY_9},
    {';', KEY_SEMICOLON},
    {'=', KEY_EQUAL},
    {'[', KEY_LEFTBRACE},
    {'\\', KEY_102ND},
    {']', KEY_RIGHTBRACE},
    {'`', KEY_GRAVE},
    {'a', KEY_A},
    {'b', KEY_B},
    {'c', KEY_C},
    {'d', KEY_D},
    {'e', KEY_E},
    {'f', KEY_F},
    {'g', KEY_G},
    {'h', KEY_H},
    {'i', KEY_I},
    {'j', KEY_J},
    {'k', KEY_K},
    {'l', KEY_L},
    {'m', KEY_M},
    {'n', KEY_N},
    {'o', KEY_O},
    {'p', KEY_P},
    {'q', KEY_Q},
    {'r', KEY_R},
    {'s', KEY_S},
    {'t', KEY_T},
    {'u', KEY_U},
    {'v', KEY_V},
    {'w', KEY_W},
    {'x', KEY_X},
    {'y', KEY_Y},
    {'z', KEY_Z}
};

/// All valid shifted characters
/// Used for mapping the char representation to the uinput keycode
const struct key_char SHIFTED_KEYS[] = {
    {'!', KEY_1},
    {'"', KEY_2},
    {'$', KEY_4},
    {'%', KEY_5},
    {'&', KEY_7},
    {'(', KEY_9},
    {')', KEY_0},
    {'*', KEY_8},
    {'+', KEY_EQUAL},
    {':', KEY_SEMICOLON},
    {'<', KEY_COMMA},
    {'>', KEY_DOT},
    {'?', KEY_SLASH},
    {'@', KEY_APOSTROPHE},
    {'A', KEY_A},
    {'B', KEY_B},
    {'C', KEY_C},
    {'D', KEY_D},
    {'E', KEY_E},
    {'F', KEY_F},
    {'G', KEY_G},
    {'H', KEY_H},
    {'I', KEY_I},
    {'J', KEY_J},
    {'K', KEY_K},
    {'L', KEY_L},
    {'M', KEY_M},
    {'N', KEY_N},
    {'O', KEY_O},
    {'P', KEY_P},
    {'Q', KEY_Q},
    {'R', KEY_R},
    {'S', KEY_S},
    {'T', KEY_T},
    {'U', KEY_U},
    {'V', KEY_V},
    {'W', KEY_W},
    {'X', KEY_X},
    {'Y', KEY_Y},
    {'Z', KEY_Z},
    {'^', KEY_6},
    {'_', KEY_MINUS},
    {'{', KEY_LEFTBRACE},
    {'|', KEY_102ND},
    {'}', KEY_RIGHTBRACE},
    {'~', KEY_BACKSLASH}
};

/// All valid modifier keys
/// Used for mapping the string representation to the uinput keycode
const struct key_string MODIFIER_KEYS[] = {
	{"ALT", KEY_LEFTALT},
    {"ALT_L", KEY_LEFTALT},
    {"ALT_R", KEY_RIGHTALT},
	{"CTRL", KEY_LEFTCTRL},
    {"CTRL_L", KEY_LEFTCTRL},
    {"CTRL_R", KEY_RIGHTCTRL},
	{"META", KEY_LEFTMETA},
    {"META_L", KEY_LEFTMETA},
    {"META_R", KEY_RIGHTMETA},
	{"SHIFT", KEY_LEFTSHIFT},
    {"SHIFT_L", KEY_LEFTSHIFT},
    {"SHIFT_R", KEY_RIGHTSHIFT},
	{"SUPER", KEY_LEFTMETA},
    {"SUPER_L", KEY_LEFTMETA},
    {"SUPER_R", KEY_RIGHTMETA}
};

/// All valid function keys
/// Used for mapping the string representation to the uinput keycode
const struct key_string FUNCTION_KEYS[] = {
	{"BACKSPACE", KEY_BACKSPACE},
    {"CAPSLOCK", KEY_CAPSLOCK},
    {"DELETE", KEY_DELETE},
    {"DOWN", KEY_DOWN},
    {"END", KEY_END},
    {"ENTER", KEY_ENTER},
    {"ESC", KEY_ESC},
	{"F1", KEY_F1},
    {"F10", KEY_F10},
    {"F11", KEY_F11},
    {"F12", KEY_F12},
    {"F2", KEY_F2},
    {"F3", KEY_F3},
    {"F4", KEY_F4},
    {"F5", KEY_F5},
    {"F6", KEY_F6},
    {"F7", KEY_F7},
    {"F8", KEY_F8},
    {"F9", KEY_F9},
    {"HOME", KEY_HOME},
    {"INSERT", KEY_INSERT},
    {"LEFT", KEY_LEFT},
    {"NUMLOCK", KEY_NUMLOCK},
    {"PAGEDOWN", KEY_PAGEDOWN},
    {"PAGEUP", KEY_PAGEUP},
    {"PAUSE", KEY_PAUSE},
    {"RIGHT", KEY_RIGHT},
    {"SCROLLLOCK", KEY_SCROLLLOCK},
    {"SYSRQ", KEY_SYSRQ},
	{"TAB", KEY_TAB},
    {"UP", KEY_UP}
};

/// Search the given array for a given string and return its keycode
/// @param arr Array of *keys* to be searched
/// @param len Length of arr
/// @param str The character array to be found
/// @param [out] code The keycode associated with str, if found
/// @return 0 on success, 1 if error(s)
int uinput_binary_search_string(const struct key_string * arr, size_t len, const char * str, uint16_t * code) {
    size_t lo = 0;
    size_t hi = len-1;
    while (lo <= hi) {
        // Calculate middle element index
        size_t mid = (hi + lo)/2;

        // If middle element equals target string, found!
        if (!strcmp(arr[mid].string, str)) {
            *code = arr[mid].code;
            return 0;
        }

        // If middle element less than target string, remove lower half
        if (strcmp(arr[mid].string, str) < 0) {
            lo = mid + 1;
        // If middle element greater than target string, remove upper half
        } else {
            hi = mid - 1;
        }
    }

    // If we reach here, string is not found
    return 1;
}

/// Search the given array for a given character and return its keycode
/// @param arr Array of *keys* to be searched
/// @param len Length of arr
/// @param c The character to be found
/// @param [out] code The keycode associated with c, if found
/// @return 0 on success, 1 if error(s)
int uinput_binary_search_char(const struct key_char * arr, size_t len, char c, uint16_t * code) {
    size_t lo = 0;
    size_t hi = len-1;
    while (lo <= hi) {
        // Calculate middle element index
        size_t mid = (hi + lo)/2;

        // If middle element equals target char, found!
        if (arr[mid].character == c) {
            *code = arr[mid].code;
            return 0;
        }

        // If middle element less than target char, remove lower half
        if (arr[mid].character < c) {
            lo = mid + 1;
        // If middle element greater than target char, remove upper half
        } else {
            hi = mid - 1;
        }
    }

    // If we reach here, char is not found */
    return 1;
}

/// Create socket to talk to ydotool daemon
/// @return 0 on succes, 1 if error(s)
int uinput_connect_socket() {
    const char * path_socket = "/tmp/.ydotool_socket";
    FD = socket(AF_UNIX, SOCK_STREAM, 0);

    if (FD == -1) {
        fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path_socket, sizeof(addr.sun_path)-1);

    if (connect(FD, (struct sockaddr *)&addr, sizeof(addr))) {
        fprintf(stderr, "Failed to connect to socket: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}

#define die(str, args...) do { \
        perror(str); \
        exit(EXIT_FAILURE); \
    } while(0)

// Initialise the input device
int uinput_init() {
    // Attempt to connect to ydotoold backend if running
    if (!uinput_connect_socket()) {
        printf("Using ydotoold backend\n");
        return 0;
    }

    // Check write access to uinput driver device
    if (access("/dev/uinput", W_OK)) {
        fprintf(stderr, "Do not have access to write to /dev/uinput!\n"
            "Try running as root\n");
        return 1;
    }

    // Confirm availability of uinput kernel module
    struct utsname uname_buffer;
    CHECK( uname(&uname_buffer) );
    char kernel_mod_dir[50] = "/lib/modules/";
    strcat(kernel_mod_dir, uname_buffer.release);
    struct stat stats;
    stat(kernel_mod_dir, &stats);
    if (!S_ISDIR(stats.st_mode)) {
        fprintf(stderr, "Dir (%s) doesn't exist!\n"
            "Have you recently updated your kernel version?\n"
            "Restart your system to use new kernel modules\n", kernel_mod_dir);
        return 1;
    }

    // Open uinput driver device
    CHECK( (FD = open("/dev/uinput", O_WRONLY|O_NONBLOCK)) );

#if 0 
    // Events/Keys setup
    for (int i = 0; i != NUM_KEYCODES; ++i) {
        CHECK( ioctl(FD, UI_SET_KEYBIT, KEYCODES[i]) );
    }
    for (int i = 0; i != NUM_EVCODES; ++i) {
        CHECK( ioctl(FD, UI_SET_EVBIT, EVCODES[i]) );
    }

    // uinput device setup
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
#endif


	//screenshot key commands
    	if(ioctl(FD, UI_SET_EVBIT, EV_KEY) < 0)
        	die("error: ioctl");
    	if(ioctl(FD, UI_SET_KEYBIT, KEY_S) < 0)
	        die("error: ioctl");
    	if(ioctl(FD, UI_SET_KEYBIT, KEY_LEFTMETA) < 0)
	        die("error: ioctl");
//    	if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
   	if(ioctl(FD, UI_SET_KEYBIT, BTN_TOUCH) < 0)
 	      die("error: ioctl");

	// for mouse
//    	if(ioctl(FD, UI_SET_EVBIT, EV_REL) < 0)
//        	die("error: ioctl");
//    	if(ioctl(FD, UI_SET_RELBIT, REL_X) < 0)
//        	die("error: ioctl");
//    	if(ioctl(FD, UI_SET_RELBIT, REL_Y) < 0)
//        	die("error: ioctl");

    	if(ioctl(FD, UI_SET_EVBIT, EV_ABS) < 0)
        	die("error: ioctl");
    	if(ioctl(FD, UI_SET_ABSBIT, ABS_X) < 0)
        	die("error: ioctl");
    	if(ioctl(FD, UI_SET_ABSBIT, ABS_Y) < 0)
        	die("error: ioctl");

    	memset(&uidev, 0, sizeof(uidev));
    	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "ydotool virtual device");
    	uidev.id.bustype = BUS_USB;
    	uidev.id.vendor  = 0x1;
    	uidev.id.product = 0x1;
    	uidev.id.version = 1;

	uidev.absmin[ABS_X] = 0;
    	uidev.absmax[ABS_X] = 800;
//    	uidev.absmax[ABS_X] = 1920  ;
	uidev.absmin[ABS_Y] = 0;
    	uidev.absmax[ABS_Y] = 480;
//    	uidev.absmax[ABS_Y] = 1080;

    	if(write(FD, &uidev, sizeof(uidev)) < 0)
        	die("error: write");

    	if(ioctl(FD, UI_DEV_CREATE) < 0)
        	die("error: ioctl");


    // Wait for device to come up
    usleep(1000000);

    return 0;
}

// Delete the input device
int uinput_destroy() {
    if (FD != -1) {
        ioctl(FD, UI_DEV_DESTROY);
        close(FD);
        FD = -1;
    }
    return 0;
}

int uinput_keychar_to_keycode(const char c, uint16_t * keycode, uint8_t * shifted) {
    // Search normal keys
    if (!uinput_binary_search_char(NORMAL_KEYS, NUM_NORMAL_KEYS, c, keycode)) {
        return 0;
    }
    // Search shifted keys
    if (!uinput_binary_search_char(SHIFTED_KEYS, NUM_SHIFTED_KEYS, c, keycode)) {
        *shifted = 1;
        return 0;
    }

    fprintf(stderr, "Failed to find key char %c!\n", c);
    return 1;
}

int uinput_keystring_to_keycode(const char * key_string, uint16_t * keycode, uint8_t * shifted) {
    *shifted = 0;

    if (strlen(key_string) == 1) {
        return uinput_keychar_to_keycode(key_string[0], keycode, shifted);
    }

    // Search modifier keys
    if (!uinput_binary_search_string(MODIFIER_KEYS, NUM_MODIFIER_KEYS, key_string, keycode)) {
        return 0;
    }
    // Search function keys
    if (!uinput_binary_search_string(FUNCTION_KEYS, NUM_FUNCTION_KEYS, key_string, keycode)) {
        return 0;
    }

    fprintf(stderr, "Failed to find key string %s!\n", key_string);
    return 1;
}

int uinput_enter_key(const char * key_string, int32_t value) {
    uint8_t shifted = 0;
    uint16_t keycode = 0;

    if (!uinput_keystring_to_keycode(key_string, &keycode, &shifted)) {
        if (shifted) {
            if (uinput_send_key(KEY_LEFTSHIFT, value) || uinput_send_key(keycode, value)) {
                return 1;
            }
        } else {
            if (uinput_send_key(keycode, value)) {
                return 1;
            }
        }
        return 0;
    }

    return 1;
}

// Emulate typing the given character on the vitual device
int uinput_enter_char(char c) {
    uint8_t shifted = 0;
    uint16_t keycode = 0;

    if (!uinput_keychar_to_keycode(c, &keycode, &shifted)) {
        if (shifted) {
            if (uinput_send_shifted_keypress(keycode)) {
                return 1;
            }
        } else {
            if (uinput_send_keypress(keycode)) {
                return 1;
            }
        }
        return 0;
    }

    return 1;
}

// Trigger an input event
int uinput_emit(uint16_t type, uint16_t code, int32_t value) {
    struct input_event ie = {
        // Ignore timestamp values
        {0,0},
        type,
        code,
        value
    };

    if (FD == -1) {
        if (uinput_init()) {
            return 1;
        }
    }

    CHECK( write(FD, &ie, sizeof(ie)) );

    // Allow processing time for uinput before sending next event
    usleep( 50 );

    return 0;
}

// Single key event and report
int uinput_send_key(uint16_t code, int32_t value) {
    if (uinput_emit(EV_KEY, code, value) || uinput_emit(EV_SYN, SYN_REPORT, 0)) {
        return 1;
    }
    return 0;
}

// Emulate a quick key press
int uinput_send_keypress(uint16_t code) {
    // send press followed by release
    if (uinput_send_key(code, 1) || uinput_send_key(code, 0)) {
        return 1;
    }
    return 0;
}

// Emulate a shifted key press
int uinput_send_shifted_keypress(uint16_t code) {
    // Send shift press, keypress, shift release
    if (uinput_send_key(KEY_LEFTSHIFT, 1)
            || uinput_send_keypress(code)
            || uinput_send_key(KEY_LEFTSHIFT, 0)
            ) {
        return 1;
    }
    return 0;
}

// Move the cursor to a given (x,y) position
int uinput_move_mouse(int32_t x, int32_t y) {
    if (uinput_emit(EV_ABS, ABS_X, x)
            || uinput_emit(EV_ABS, ABS_Y, y)
            || uinput_emit(EV_SYN, SYN_REPORT, 0)
            ) {
        return 1;
    }
    return 0;
}

// Move the cursor a given (x,y) relative to the current position
int uinput_relative_move_mouse(int32_t x, int32_t y) {
    if (x) {
        if (uinput_emit(EV_REL, REL_X, x)) {
            return 1;
        }
    }
    if (y) {
        if (uinput_emit(EV_REL, REL_Y, y)) {
            return 1;
        }
    }
    if (uinput_emit(EV_SYN, SYN_REPORT, 0)) {
        return 1;
    }
    return 0;
}

int uinput_touch_tap_event(int x, int y)
{
//	uinput_emit(EV_KEY, BTN_TOUCH, 1);
	uinput_emit(EV_ABS, ABS_X, x);
	uinput_emit(EV_ABS, ABS_Y, y);
	uinput_emit(EV_SYN, SYN_REPORT, 0);

	uinput_emit(EV_KEY, BTN_TOUCH, 1);
	uinput_emit(EV_SYN, SYN_REPORT, 0);
	usleep (500);

	// Report KEY - RELEASE event
	uinput_emit(EV_KEY, BTN_TOUCH, 0);
	uinput_emit(EV_SYN, SYN_REPORT, 0);
}

int uinput_touch_swipe_event(int startx, int starty, int endx, int endy, int duration)
{
	uinput_touch_tap_event (startx, starty);
	usleep (500);

//	uinput_emit(EV_KEY, BTN_TOUCH, 1);
        uinput_emit(EV_ABS, ABS_X, startx);
        uinput_emit(EV_ABS, ABS_Y, starty);
	uinput_emit(EV_KEY, BTN_TOUCH, 1);
        uinput_emit(EV_SYN, SYN_REPORT, 0);

//      stroke_emit(EV_KEY, BTN_LEFT, 1);
//        uinput_emit(EV_KEY, BTN_TOUCH, 1);
//        uinput_emit(EV_SYN, SYN_REPORT, 0);

//	uinput_emit(EV_KEY, BTN_TOUCH, 0);
//        uinput_emit(EV_SYN, SYN_REPORT, 0);

	usleep(duration);

//        uinput_emit(EV_KEY, BTN_TOUCH, 0);
        uinput_emit(EV_ABS, ABS_X, endx);
        uinput_emit(EV_ABS, ABS_Y, endy);
        uinput_emit(EV_SYN, SYN_REPORT, 0);

	uinput_emit(EV_KEY, BTN_TOUCH, 0);
        uinput_emit(EV_SYN, SYN_REPORT, 0);
}

