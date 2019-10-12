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

#ifndef __UINPUT_H__
#define __UINPUT_H__

#include <stdint.h>
#include <linux/uinput.h>

struct uinput_raw_data {
    uint16_t type;
    uint16_t code;
    int32_t value;
} __attribute__((packed));

void uinput_destroy();
void uinput_enter_char(char c);
void uinput_emit(uint16_t type, uint16_t code, int32_t value);
void uinput_send_key(uint16_t code, int32_t value);
void uinput_send_keypress(uint16_t code);
void uinput_send_shifted_keypress(uint16_t code);
void uinput_move_mouse(int32_t x, int32_t y);
void uinput_relative_move_mouse(int32_t x, int32_t y);

#endif // __UINPUT_H__
