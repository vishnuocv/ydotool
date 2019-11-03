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
 * @file mouse.c
 * @author Harry Austen
 * @brief Implementation of function for emulating moving your mouse around
 */

/* System includes */
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Local includes */
#include "mouse.h"
#include "uinput.h"

int mouse_print_usage() {
    fprintf(stderr,
        "Usage: mouse [--delay <ms>] <x> <y>\n"
        "    --help      Show this help\n"
        "    --delay ms  Delay time before start moving (default = 100ms)\n");
    return 1;
}

int mouse_run(int argc, char ** argv) {
	uint32_t time_delay = 100;
    int opt = 0;

    enum optlist_t {
        opt_help,
        opt_delay
    };

    static struct option long_options[] = {
        {"help",  no_argument,       NULL, opt_help  },
        {"delay", required_argument, NULL, opt_delay }
    };

    while ((opt = getopt_long_only(argc, argv, "hd:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
            case opt_delay:
                time_delay = (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case opt_help:
            case '?':
                return mouse_print_usage();
        }
    }

    int extra_args = argc - optind;
    if (extra_args != 2) {
        fprintf(stderr, (extra_args > 2) ? "Too may arguments!\n" : "Not enough arguments!\n");
        return mouse_print_usage();
    }

	if (time_delay)
		usleep(time_delay * 1000);

	int32_t x = (int32_t)strtol(argv[optind++], NULL, 10);
	int32_t y = (int32_t)strtol(argv[optind],   NULL, 10);

	if (!strchr(argv[0], '_')) {
		if (uinput_move_mouse(x, y)) {
            return 1;
        }
	} else {
        if (uinput_relative_move_mouse(x, y)) {
            return 1;
        }
    }

	return 0;
}
