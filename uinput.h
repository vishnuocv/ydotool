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

/// @file uinput.h
/// @author Harry Austen
/// @brief Interface for functions for emulating input events

#ifndef __UINPUT_H__
#define __UINPUT_H__

// System includes
#include <stdint.h>
#include <linux/uinput.h>

/// Number of normal keys
#define NUM_NORMAL_KEYS 51
/// Number of shifted keys
#define NUM_SHIFTED_KEYS 46
/// Number of modifier keys
#define NUM_MODIFIER_KEYS 15
/// Number of function keys
#define NUM_FUNCTION_KEYS 31

/// @brief uinput event information
struct uinput_raw_data {
    /// The type of input event (e.g. key input or mouse movement)
    uint16_t type;
    /// An integer representing the key to input or direction to move in
    uint16_t code;
    /// 1 for key press, 0 for key release or any integer value for absolute/relative mouse movement in pixels
    int32_t value;
};

/// @brief Represents a single keyboard character
/// @details Used to convert between the char and the integer keycode
struct key_char {
    /// The character representation of the key
    char character;
    /// The Linux uinput keycode representing the associated key
    uint16_t code;
};

/// @brief Represents a single keyobard function/modifier key
/// @details Used to convert between the string representation of the key and the integer keycode
struct key_string {
    /// String representation of the key
    char string[11];
    /// The Linux uinput keycode representing the associated key
    uint16_t code;
};

/// @brief Array of all normal (non-shifted) character keys
extern const struct key_char NORMAL_KEYS[NUM_NORMAL_KEYS];

/// @brief Array of all shifted character keys
extern const struct key_char SHIFTED_KEYS[NUM_SHIFTED_KEYS];

/// @brief Array of all modifier keys
extern const struct key_string MODIFIER_KEYS[NUM_MODIFIER_KEYS];

/// @brief Array of all function keys
extern const struct key_string FUNCTION_KEYS[NUM_FUNCTION_KEYS];

/// @brief Initialise uinput device
/// @return 0 on success, 1 if error(s)
int uinput_init();

/// @brief Close uinput device if open
/// @return 0 on success, 1 if error(s)
int uinput_destroy();

/// @brief Emulate entering the given char using a virtual input device
/// @param c The character to be entered
/// @return 0 on success, 1 if error(s)
int uinput_enter_char(char c);

/// @brief Emulate a single uinput event
/// @param type The type of input event (e.g. key input or mouse movement)
/// @param code An integer representing the key to input or direction to move in
/// @param value 1 for key press, 0 for key release or any integer value for absolute/relative mouse movement in pixels
/// @return 0 on success, 1 if error(s)
int uinput_emit(uint16_t type, uint16_t code, int32_t value);

/// @brief Emulate a single key event for the given string representation of a key
/// @param key_string Character array representing the key to be pressed/released
/// @param value 1 for press, 0 for release
/// @return 0 on success, 1 if error(s)
int uinput_enter_key(const char * key_string, int32_t value);

/// @brief Convert char representation of a key to the associated integer keycode
/// @param [in] c Character representing the key
/// @param [out] keycode The integer keycode for the given key
/// @param [out] shifted 1 if keycode represents a shifted key, 0 if normal
/// @return 0 on success, 1 if error(s)
int uinput_keychar_to_keycode(const char c, uint16_t * keycode, uint8_t * shifted);

/// @brief Convert string/char representation of a key to the associated integer keycode
/// @param [in] key_string Character array representing the key
/// @param [out] keycode The integer keycode for the given key
/// @param [out] shifted 1 if keycode represents a shifted key, 0 if normal
/// @return 0 on success, 1 if error(s)
int uinput_keystring_to_keycode(const char * key_string, uint16_t * keycode, uint8_t * shifted);

/// @brief Send a particular key input event (press or release)
/// @param code The integer representation of the associated key
/// @param value 1 for press, 0 for release
/// @return 0 on success, 1 if error(s)
int uinput_send_key(uint16_t code, int32_t value);

/// @brief Emulate a keypress of a given keycode (i.e. press & release)
/// @param code The integer representation of the associated key
/// @return 0 on success, 1 if error(s)
int uinput_send_keypress(uint16_t code);

/// @brief Emulate a keypress of a given keycode whilst holding the shift key
/// @param code The integer representation of the associated key
/// @return 0 on success, 1 if error(s)
int uinput_send_shifted_keypress(uint16_t code);

/// @brief Move the mouse to a given x and y pixel position
/// @param x Horizontal pixel position
/// @param y Vertical pixel position
/// @return 0 on success, 1 if error(s)
int uinput_move_mouse(int32_t x, int32_t y);

/// @brief Move the mouse a given amount in the x and y directions
/// @param x Relative horizontal pixel movement
/// @param y Relative vertical pixel movement
/// @return 0 on success, 1 if error(s)
int uinput_relative_move_mouse(int32_t x, int32_t y);

int uinput_touchevent(int x, int y);

#endif // __UINPUT_H__
