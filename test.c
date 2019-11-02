/*
    This file is part of ydotool.
	Copyright (C) 2019 Harry Austen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/**
 * @file test.c
 * @author Harry Austen
 * @brief Program for testing the ydotool code
 */

/* System includes */
#include <string.h>
#include <stdio.h>

/* Local includes */
#include "uinput.h"

int uinput_test_array_order() {
    int ret = 0;

    for (size_t i = 1; i != NUM_NORMAL_KEYS; ++i) {
        if (NORMAL_KEYS[i].character < NORMAL_KEYS[i-1].character) {
            printf("%c < %c\n", NORMAL_KEYS[i].character, NORMAL_KEYS[i-1].character);
            ret++;
        }
    }

    for (size_t i = 1; i != NUM_SHIFTED_KEYS; ++i) {
        if (SHIFTED_KEYS[i].character < SHIFTED_KEYS[i-1].character) {
            printf("%c < %c\n", SHIFTED_KEYS[i].character, SHIFTED_KEYS[i-1].character);
            ret++;
        }
    }

    for (size_t i = 1; i != NUM_MODIFIER_KEYS; ++i) {
        if (strcmp(MODIFIER_KEYS[i].string, MODIFIER_KEYS[i-1].string) < 0) {
            printf("%s < %s\n", MODIFIER_KEYS[i].string, MODIFIER_KEYS[i-1].string);
            ret++;
        }
    }

    for (size_t i = 1; i != NUM_FUNCTION_KEYS; ++i) {
        if (strcmp(FUNCTION_KEYS[i].string, FUNCTION_KEYS[i-1].string) < 0) {
            printf("%s < %s\n", FUNCTION_KEYS[i].string, FUNCTION_KEYS[i-1].string);
            ret++;
        }
    }

    return ret;
}

int uinput_test_keystring_to_keycode() {
    int ret = 0;
    uint16_t code = 0;
    uint8_t shifted = 0;

    for (size_t i = 0; i != NUM_NORMAL_KEYS; ++i) {
        if (uinput_keystring_to_keycode(&NORMAL_KEYS[i].character, &code, &shifted)) {
            printf("'%c' NOT FOUND!\n", NORMAL_KEYS[i].character);
        } else if (code != NORMAL_KEYS[i].code) {
            printf("Code does not match for '%c'. Got %d, expected %d\n", NORMAL_KEYS[i].character, code, NORMAL_KEYS[i].character);
        } else if (shifted != 0) {
            printf("Returned shifted character, but expected normal character '%c'\n", NORMAL_KEYS[i].character);
        } else {
            continue;
        }
        ret++;
    }

    for (size_t i = 0; i != NUM_SHIFTED_KEYS; ++i) {
        if (uinput_keystring_to_keycode(&SHIFTED_KEYS[i].character, &code, &shifted)) {
            printf("'%c' NOT FOUND!\n", SHIFTED_KEYS[i].character);
        } else if (code != SHIFTED_KEYS[i].code) {
            printf("Code does not match for '%c'. Got %d, expected %d\n", SHIFTED_KEYS[i].character, code, SHIFTED_KEYS[i].character);
        } else if (shifted != 1) {
            printf("Returned non-shifted character, but expected shifted character '%c'\n", SHIFTED_KEYS[i].character);
        } else {
            continue;
        }
        ret++;
    }

    for (size_t i = 0; i != NUM_MODIFIER_KEYS; ++i) {
        if (uinput_keystring_to_keycode(MODIFIER_KEYS[i].string, &code, &shifted)) {
            printf("%s NOT FOUND!\n", MODIFIER_KEYS[i].string);
        } else if ( code != MODIFIER_KEYS[i].code ) {
            printf("Code does not match for %s. Got %d, expected %d\n", MODIFIER_KEYS[i].string, code, MODIFIER_KEYS[i].code);
        } else if (shifted != 0) {
            printf("Returned shifted key, but expected normal key '%s'\n", MODIFIER_KEYS[i].string);
        } else {
            continue;
        }
        ret++;
    }

    for (size_t i = 0; i != NUM_FUNCTION_KEYS; ++i) {
        if (uinput_keystring_to_keycode(FUNCTION_KEYS[i].string, &code, &shifted)) {
            printf("%s NOT FOUND!\n", FUNCTION_KEYS[i].string);
        } else if ( code != FUNCTION_KEYS[i].code ) {
            printf("Code does not match for %s. Got %d, expected %d\n", FUNCTION_KEYS[i].string, code, FUNCTION_KEYS[i].code);
        } else if (shifted != 0) {
            printf("Returned shifted key, but expected normal key '%s'\n", FUNCTION_KEYS[i].string);
        } else {
            continue;
        }
        ret++;
    }

    return ret;
}

int uinput_test() {
    int ret = 0;

    ret += uinput_test_array_order();
    ret += uinput_test_keystring_to_keycode();

    return ret;
}

int main() {
    int ret = 0;

    /**
     * @todo Add tests for different areas of the ydotool codebase
     */

    ret += uinput_test();

    if (ret) {
        printf("FAILED %d tests\n", ret);
    } else {
        printf("PASSED!\n");
    }

    return ret;
}
