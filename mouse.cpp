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

// Local includes
#include "ydotool.hpp"
// C++ system includes
#include <iostream>
#include <cstring>
extern "C" {
/* System includes */
#include <getopt.h>
#include <unistd.h>
/* Local includes */
#include "uinput.h"
}

void mouse_help(){
	std::cerr << "Usage: mouse [--delay <ms>] <x> <y>\n"
			<< "  --help                Show this help.\n"
			<< "  --delay ms            Delay time before start moving. Default 100ms." << std::endl;
}

int mouse_run(int argc, char ** argv) {
	int time_delay = 100;
    int opt = 0;

    typedef enum {
        opt_help,
        opt_delay
    } optlist_t;

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
            std::cerr << "Too many args!" << std::endl;
        } else {
            std::cerr << "Too few args!" << std::endl;
        }
    }

	if (time_delay)
		usleep(time_delay * 1000);

	int32_t x = (int32_t)strtol(argv[optind++], nullptr, 10);
	int32_t y = (int32_t)strtol(argv[optind],   nullptr, 10);

	if (!strchr(argv[0], '_')) {
		uinput_move_mouse(x, y);
	} else {
        uinput_relative_move_mouse(x, y);
    }

	return argc;
}
