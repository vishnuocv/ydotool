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
 * @file type.c
 * @author Harry Austen
 * @brief Implementation of function for emulating typing text
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

static const char * usage =
    "Usage: type [--delay milliseconds] [--key-delay milliseconds] [--args N] [--file <filepath>] <things to type>\n"
    "    --help                    Show this help\n"
    "    --delay milliseconds      Delay time before start typing\n"
    "    --key-delay milliseconds  Delay time between keystrokes (default = 12ms)\n"
    "    --file filepath           Specify a file, the contents of which will be be typed as if passed as an argument. The filepath may also be '-' to read from stdin\n";

int type_text(char * text) {
	for (int i=0; text[i] != '\0'; ++i) {
        if (uinput_enter_char(text[i])) {
            return 1;
        }
	}
    return 0;
}

int type_run(int argc, char ** argv) {
	int time_delay = 100;
    /*
    int time_keydelay = 12;
	char file_path[100] = "";
    */
    int opt = 0;

    enum optlist_t {
        opt_help,
        opt_delay,
        /*
        opt_key_delay,
        opt_file,
        */
    };

    static struct option long_options[] = {
        {"help",      no_argument,       NULL, opt_help     },
        {"delay",     required_argument, NULL, opt_delay    },
        /*
        {"key-delay", required_argument, NULL, opt_key_delay},
        {"file",      required_argument, NULL, opt_file     },
        */
    };

    while ((opt = getopt_long_only(argc, argv, "hd:k:f:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
            case opt_delay:
                time_delay = strtoul(optarg, NULL, 10);
                break;
            /*
            case 'k':
            case opt_key_delay:
                time_keydelay = strtoul(optarg, NULL, 10);
                break;
            case 'f':
            case opt_file:
                strcat(file_path, optarg);
                break;
            */
            case 'h':
            case opt_help:
            case '?':
                fprintf(stderr, usage);
                return 1;
        }
    }

    /*
	FILE * fd = NULL;

	if (strcmp(file_path, "")) {
		if (!strcmp(file_path, "-")) {
            printf("Setting fd\n");
			fd = stdin;
            printf("Set\n");
			fprintf(stderr, "ydotool: type: reading from stdin\n");
		} else {
			fd = fopen(file_path, O_RDONLY);

			if (!fd) {
				fprintf(stderr, "ydotool: type: error: failed to open %s: %s\n", file_path, strerror(errno));
				return 1;
			}
		}
	}
    */

	if (time_delay)
		usleep(time_delay * 1000);

    /*
	if (fd) {
        printf("Seeking\n");
        if (fseek(fd, 0L, SEEK_END)) {
            printf("FAIL\n");
        }
        printf("Seeked\n");
        size_t len = ftell(fd);
        if (fseek(fd, 0L, SEEK_SET)) {
            printf("FAIL2\n");
        }
        char * buf = malloc(sizeof(char) * (len + 1));

		while (fgets(buf, len, fd)) {
            if (type_text(buf)) {
                return 1;
            }
		}

		if (fclose(fd)) {
            printf("FAIL3\n");
        }
	} else {
    */
        /* Check for input */
        int extra_args = argc - optind;
        if (!extra_args) {
            fprintf(stderr, "Not enough args!\n");
            fprintf(stderr, usage);
            return 1;
        }

        /* Sum length of args */
        size_t len = 0;
        for (; optind != argc; ++optind) {
            len += strlen(argv[optind]);
        }

        /*
         * Allocate character array buffer
         * "+1" to allow space for null terminating byte
         */
        char * buf = malloc(sizeof(char) * (len + 1));
        optind -= extra_args;

        /* Concatenate args into buffer */
        for (; optind != argc; ++optind) {
            strcat(buf, argv[optind]);
        }

        /* Emulate keyboard input of buffer characters */
        if (type_text(buf)) {
            return 1;
        }
    /*
	}
    */

	return 0;
}
