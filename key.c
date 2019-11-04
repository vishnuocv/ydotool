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

/**
 * @file key.c
 * @author Harry Austen
 * @brief Implementation of function to emulate entering key sequences
 */

// System includes
#include <getopt.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Local Includes
#include "key.h"
#include "uinput.h"

/**
 * Print usage string to stderr
 * @return 1 (error)
 */
int key_print_usage() {
    fprintf(stderr,
        "Usage: key [--delay <ms>] [--key-delay <ms>] [--repeat <times>] [--repeat-delay <ms>] <key sequence> ...\n"
        "    --help             Show this help\n"
        "    --delay ms         Delay time before start pressing keys (default = 100ms)\n"
        "    --key-delay ms     Delay time between keystrokes (default = 12ms)\n"
        "    --repeat times     Times to repeat the key sequence\n"
        "    --repeat-delay ms  Delay time between repetitions (default = 0ms)\n"
        "Each key sequence can be any number of modifiers and keys, separated by plus (+)\nFor example: alt+r Alt+F4 CTRL+alt+f3 aLT+1+2+3 ctrl+Backspace\n");
    return 1;
}

/**
 * Press all keys, then release all keys
 * @param key_string Sequence of string representations of keys to be pressed together, separated by '+'
 * @return 0 on success, 1 if error(s)
 */
int enter_keys(char * key_string) {
    char * ptr = strtok(key_string, "+");
    while (ptr) {
        if (uinput_enter_key(ptr, 1)) {
            return 1;
        }
        ptr = strtok(NULL, "+");
    }
    ptr = strtok(key_string, "+");
    while (ptr) {
        if (uinput_enter_key(ptr, 0)) {
            return 1;
        }
        ptr = strtok(NULL, "+");
    }
    return 0;
}

int key_run(int argc, char ** argv) {
	uint32_t time_delay = 100;
	uint64_t repeats = 1;
    int opt = 0;

    enum optlist_t {
        opt_help,
        opt_delay,
        opt_repeat,
    };

    static struct option long_options[] = {
        {"help",   no_argument,       NULL, opt_help  },
        {"delay",  required_argument, NULL, opt_delay },
        {"repeat", required_argument, NULL, opt_repeat},
    };

    while((opt = getopt_long_only(argc, argv, "hd:r:", long_options, NULL)) != -1 )
    {
        switch (opt)
        {
            case 'd':
            case opt_delay:
                time_delay = (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case 'r':
            case opt_repeat:
                repeats = strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case opt_help:
            case '?':
                return key_print_usage();
        }
    }

    if (argc == optind) {
        fprintf(stderr, "Not enough args!\n");
        return key_print_usage();
    }

	if (time_delay) {
		usleep(time_delay * 1000);
    }

    const int first_arg = optind;

    while (repeats--) {
        for (; argc != optind; optind++) {
            if (enter_keys(argv[optind])) {
                return 1;
            }
        }
        optind = first_arg;
    }

	return 0;
}
