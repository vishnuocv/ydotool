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
 * @file click.c
 * @author Harry Austen
 * @brief Emulate clicking the given mouse button
 */

/* Local includes */
#include "uinput.h"
/* System includes */
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static const char * usage =
    "Usage: click [--delay <ms>] <button>\n"
    "    --help      Show this help\n"
    "    --delay ms  Delay time before start clicking (default = 100ms)\n"
    "    button      1: left\n"
    "                2: right\n"
    "                3: middle\n";

int click_run(int argc, char ** argv) {
	int time_delay = 100;
    int opt = 0;

    enum optlist_t {
        opt_help,
        opt_delay
    };

    static struct option long_options[] = {
        {"help",  no_argument,       NULL, opt_help },
        {"delay", required_argument, NULL, opt_delay}
    };

    while ((opt = getopt_long_only(argc, argv, "hd:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
            case opt_delay:
                time_delay = strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case opt_help:
            case '?':
                fprintf(stderr, usage);
                return 1;
        }
    }

    int extra_args = argc - optind;
    if (extra_args != 1) {
        if (extra_args > 1 ) {
            fprintf(stderr, "Too many arguments!\n");
        } else {
            fprintf(stderr, "Not enough arguments!\n");
        }
        fprintf(stderr, usage);
        return 1;
    }

    int button = strtoul(argv[optind], NULL, 10);
	int keycode = BTN_LEFT;

	switch (button) {
        case 1:
            break;
		case 2:
			keycode = BTN_RIGHT;
			break;
		case 3:
			keycode = BTN_MIDDLE;
			break;
		default:
            fprintf(stderr, "Invalid button argument!\n");
            fprintf(stderr, usage);
            return 1;
	}

	if (time_delay) {
		usleep(time_delay * 1000);
    }

    if (uinput_send_keypress(keycode)) {
        return 1;
    }

	return 0;
}
