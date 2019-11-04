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
 * @file ydotool.c
 * @author Harry Austen
 * @brief Main entry point to the ydotool program. Uses the first argument to run the relevant command
 */

/* System includes */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

/* Local includes */
#include "click.h"
#include "key.h"
#include "mouse.h"
#include "type.h"
#include "uinput.h"

/**
 * Entrypoint of the ydotool program
 * @param argc Nuber of input arguments
 * @param argv Array of input arguments
 * @return 0 on success, 1 if error(s)
 */
int main(int argc, char ** argv) {
	int ret = 0;

	if (argc < 2
            || !strncmp(argv[1], "-h", 2)
            || !strncmp(argv[1], "--h", 3)
            || !strcmp(argv[1], "help")
       ) {
		fprintf(stderr,
                "Usage: %s <cmd> <args>\n"
                "Available commands:\n"
                "    click\n"
                "    key\n"
                "    mouse\n"
                "    type\n",
                argv[0]);
		return 1;
	} else {
        /* First argument dealt with, increment arg pointer */
        argv++;
        argc--;

        /* Check which command to run */
        if ( !strcmp(argv[0], "click") ) {
            ret = click_run(argc, argv);
        } else if ( !strcmp(argv[0], "key") ) {
            ret = key_run(argc, argv);
        } else if ( !strcmp(argv[0], "mouse") ) {
            ret = mouse_run(argc, argv);
        } else if ( !strcmp(argv[0], "type") ) {
            ret = type_run(argc, argv);
        } else {
            fprintf(stderr, "ydotool: Unknown option: %s\n"
                    "Run ydotool help for a list of arguments\n", argv[0]);
            ret = 1;
        }
    }

    ret += uinput_destroy();

	return ret;
}
