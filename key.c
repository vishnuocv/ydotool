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

// Local Includes
#include "key.h"
#include "uinput.h"
// System includes
#include <getopt.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void key_help() {
	fprintf(stderr, "Usage: key [--delay <ms>] [--key-delay <ms>] [--repeat <times>] [--repeat-delay <ms>] <key sequence> ...\n\t--help\t\tShow this help.\n\t--delay ms\tDelay time before start pressing keys. Default 100ms.\n\t--key-delay ms\tDelay time between keystrokes. Default 12ms.\n\t--repeat times\t\tTimes to repeat the key sequence.\n\t--repeat-delay ms\tDelay time between repetitions. Default 0ms.\nEach key sequence can be any number of modifiers and keys, separated by plus (+)\nFor example: alt+r Alt+F4 CTRL+alt+f3 aLT+1+2+3 ctrl+Backspace\nSince we are emulating keyboard input, combination like Shift+# is invalid.\nBecause typing a `#' involves pressing Shift and 3.\n");
}

/* TODO: Press all keys then release all keys */
void enter_keys(char * key_string) {
    char * ptr = strtok(key_string, "+");
    while (ptr != NULL) {
        uinput_enter_key(ptr);
        ptr = strtok(NULL, "+");
    }
}

int key_run(int argc, char ** argv) {
	int time_delay = 100;
	int repeats = 1;
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
                time_delay = strtoul(optarg, NULL, 10);
                break;
            case 'r':
            case opt_repeat:
                repeats = strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case opt_help:
            case '?':
                key_help();
                return -1;
        }
    }

    if (argc == optind) {
        fprintf(stderr, "Not enough args!\n");
        key_help();
        return -1;
    }

	if (time_delay) {
		usleep(time_delay * 1000);
    }

    const int first_arg = optind;

    while (repeats--) {
        for (; argc != optind; optind++) {
            enter_keys(argv[optind]);
        }
        optind = first_arg;
    }

	return argc;
}
