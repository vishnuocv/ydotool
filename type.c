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
#include "type.h"
#include "uinput.h"
// System includes
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int time_keydelay = 12;

void type_help(){
	fprintf(stderr, "Usage: type [--delay milliseconds] [--key-delay milliseconds] [--args N] [--file <filepath>] <things to type>\n\t--help\t\tShow this help\n\t--delay milliseconds\tDelay time before start typing\n\t--key-delay milliseconds\tDelay time between keystrokes. Default 12ms.\n\t--file filepath\tSpecify a file, the contents of which will be be typed as if passed as an argument. The filepath may also be '-' to read from stdin.\n");
}

void type_text(char * text) {
	for (int i=0; text[i] != '\0'; ++i) {
        uinput_enter_char(text[i]);
	}
}

int type_run(int argc, char ** argv) {
	int time_delay = 100;
	char file_path[100] = "";
    int opt = 0;

    enum optlist_t {
        opt_help,
        opt_delay,
        opt_key_delay,
        opt_file
    };

    static struct option long_options[] = {
        { "help",      no_argument,       NULL, opt_help      },
        { "delay",     required_argument, NULL, opt_delay     },
        { "key-delay", required_argument, NULL, opt_key_delay },
        { "file",      required_argument, NULL, opt_file      }
    };

    while ((opt = getopt_long_only(argc, argv, "hd:k:f:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
            case opt_delay:
                time_delay = strtoul(optarg, NULL, 10);
                break;
            case 'k':
            case opt_key_delay:
                time_keydelay = strtoul(optarg, NULL, 10);
                break;
            case 'f':
            case opt_file:
                strcat(file_path, optarg);
                break;
            case 'h':
            case opt_help:
            case '?':
                type_help();
                return -1;
        }
    }

    int extra_args = argc - optind;
    if (!extra_args) {
        fprintf(stderr, "Not enough args!\n");
        type_help();
        return -1;
    }

	int fd = -1;

	if (!strcmp(file_path, "")) {
		if (!strcmp(file_path, "-")) {
			fd = STDIN_FILENO;
			fprintf(stderr, "ydotool: type: reading from stdin\n");
		} else {
			fd = open(file_path, O_RDONLY);

			if (fd == -1) {
				fprintf(stderr, "ydotool: type: error: failed to open %s: %s\n", file_path, strerror(errno));
				return 2;
			}
		}
	}

	if (time_delay)
		usleep(time_delay * 1000);

	if (fd >= 0) {
		char buf[128];

		ssize_t rc;
		while ((rc = read(fd, &buf[0], 128))) {
			if (rc > 0) {
				type_text(buf);
			} else if (rc < 0) {
				fprintf(stderr, "ydotool: type: error: read %s failed: %s\n", file_path, strerror(errno));
				return 2;
			}
		}

		close(fd);
	} else {
        char * buf = "";
        for (; optind != argc; ++optind) {
            strcat(buf, argv[optind]);
        }
        type_text(buf);
	}

	return argc;
}
