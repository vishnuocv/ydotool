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

/* System includes */
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* Local includes */
#include "mouse.h"
#include "uinput.h"

void mouse_help(){
	fprintf(stderr, "Usage: mouse [--delay <ms>] <x> <y>\n\t--help\t\tShow this help\n\t--delay ms\tDelay time before start moving. Default 100ms.\n");
}

int mouse_run(int argc, char ** argv) {
	int time_delay = 100;
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
                time_delay = strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case opt_help:
            case '?':
                mouse_help();
                return -1;
        }
    }

    int extra_args = argc - optind;
    if (extra_args != 2) {
        if (extra_args > 2) {
            fprintf(stderr, "Too many args!\n");
            mouse_help();
            return -1;
        } else {
            fprintf(stderr, "Too few args!\n");
            mouse_help();
            return -1;
        }
    }

	if (time_delay)
		usleep(time_delay * 1000);

	int32_t x = (int32_t)strtol(argv[optind++], NULL, 10);
	int32_t y = (int32_t)strtol(argv[optind],   NULL, 10);

	if (!strchr(argv[0], '_')) {
		uinput_move_mouse(x, y);
	} else {
        uinput_relative_move_mouse(x, y);
    }

	return argc;
}
