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

/* Local includes */
#include "click.h"
#include "key.h"
#include "mouse.h"
#include "type.h"
#include "uinput.h"
/* System includes */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

static const char * usage =
    "Usage: %s <cmd> <args>\n"
    "Available commands:\n"
    "    click\n"
    "    key\n"
    "    mouse\n"
    "    recorder\n"
    "    type\n";

int main(int argc, char ** argv) {
	int ret = 0;

	if (argc < 2 || strncmp(argv[1], "-h", 2) == 0 || strncmp(argv[1], "--h", 3) == 0 || strcmp(argv[1], "help") == 0) {
		fprintf(stderr, usage, argv[0]);
		ret = 1;
	} else if ( !strcmp(argv[1], "click") ) {
		ret = click_run(argc-1, &argv[1]);
	} else if ( !strcmp(argv[1], "key") ) {
		ret = key_run(argc-1, &argv[1]);
	} else if ( !strcmp(argv[1], "mouse") ) {
		ret = mouse_run(argc-1, &argv[1]);
	} else if ( !strcmp(argv[1], "type") ) {
		ret = type_run(argc-1, &argv[1]);
	} else {
		fprintf(stderr, "ydotool: Unknown option: %s\nRun ydotool help for a list of arguments\n", argv[1]);
		ret = 1;
	}

    ret += uinput_destroy();

	return ret;
}
