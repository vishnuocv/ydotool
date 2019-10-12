#ifndef __UINPUT_H__
#define __UINPUT_H__

#include <stdint.h>
#include <linux/uinput.h>

struct uinput_raw_data {
    uint16_t type;
    uint16_t code;
    int32_t value;
} __attribute__((packed));

struct uinput_coordinate {
    int32_t x;
    int32_t y;
};

void uinput_destroy();
void uinput_emit(uint16_t type, uint16_t code, int32_t value);
void uinput_send_key(uint16_t code, int32_t value);
void uinput_send_keypress(uint16_t code);
void uinput_move_mouse(int32_t x, int32_t y);
void uinput_relative_move_mouse(int32_t x, int32_t y);

#endif // __UINPUT_H__
