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

// System includes
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

// Local includes
#include "type.h"
#include "uinput.h"

/**
 * Print usage string to stderr
 * @return 1 (error)
 */
int type_print_usage() {
    fprintf(stderr,
        "Usage: type [--delay milliseconds] [--key-delay milliseconds] [--args N] [--file <filepath>] <things to type>\n"
        "    --help                    Show this help\n"
        "    --delay milliseconds      Delay time before start typing\n"
        "    --key-delay milliseconds  Delay time between keystrokes (default = 12ms)\n"
        "    --file filepath           Specify a file, the contents of which will be be typed as if passed as an argument. The filepath may also be '-' to read from stdin\n");
    return 1;
}

/**
 * Enter characters in input string one at a time
 * @param text Array of characters to be entered
 * @return 0 on success, >0 if errors
 */
int type_text(char * text) {
	for (size_t i = 0; text[i] != '\0'; ++i) {
        if (uinput_enter_char(text[i])) {
            return 1;
        }
	}
    return 0;
}

int type_run(int argc, char ** argv) {
    /**
     * @todo Implement key delay
     */

    /*
	uint32_t time_delay = 100;
    int time_keydelay = 12;
    */
	char * file_path;
    int opt = 0;

    enum optlist_t {
        opt_help,
        /*
        opt_delay,
        opt_key_delay,
        */
        opt_file,
    };

    static struct option long_options[] = {
        {"help",      no_argument,       NULL, opt_help     },
        /*
        {"delay",     required_argument, NULL, opt_delay    },
        {"key-delay", required_argument, NULL, opt_key_delay},
        */
        {"file",      required_argument, NULL, opt_file     },
    };

    while ((opt = getopt_long_only(argc, argv, "hd:k:f:", long_options, NULL)) != -1) {
        switch (opt) {
            /*
            case 'd':
            case opt_delay:
                time_delay = (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case 'k':
            case opt_key_delay:
                time_keydelay = strtoul(optarg, NULL, 10);
                break;
            */
            case 'f':
            case opt_file:
                file_path = malloc(sizeof(char) * (strlen(optarg) + 1));
                strcat(file_path, optarg);
                break;
            case 'h':
            case opt_help:
            case '?':
                return type_print_usage();
        }
    }

    /* If filepath parameter was passed in */
	if (strcmp(file_path, "")) {
        /* Hyphen means read from stdin */
		if (!strcmp(file_path, "-")) {
            /* Allocate buffer for reading in chunks and text for holding full input */
            char * buf = malloc(sizeof(char) * 10);
            char * text = malloc(sizeof(char));

            /* Read up to 10 chars at a time from stdin */
            while (fgets(buf, 10, stdin)) {
                /* Add extra byte allocation for new chars */
                size_t len = strlen(text) + strlen(buf) + 1;
                char * tmp = realloc(text, sizeof(char) * len);
                if (tmp) {
                    text = tmp;
                } else {
                    free(text);
                    free(buf);
                    fprintf(stderr, "Failed to allocated %lu bytes for text buffer\n", len);
                    return 1;
                }

                /* Error handling */
                if (!text) {
                    fprintf(stderr, "Failed to read text from stdin\n");
                    return 1;
                }

                /* Append current buffer to full text string */
                strcat(text, buf);
            }

            /* Free up buffer memory */
            free(buf);

            /* Call uinput to type the text */
            if (type_text(text)) {
                return 1;
            }

            /* Free up text memory */
            free(text);
		} else {
            /* Open file_path file in read only mode */
            FILE * fd = fopen(file_path, "r");

			if (!fd) {
				fprintf(stderr, "ydotool: type: error: failed to open %s: %s\n", file_path, strerror(errno));
				return 1;
			}

            /* Seek to end of file */
            if (fseek(fd, 0L, SEEK_END)) {
                fprintf(stderr, "Failed to seek to end of file %s\n", file_path);
            }

            /* Grab length of file (current position) */
            int64_t len = ftell(fd);

            /* Seek back to beginning */
            if (fseek(fd, 0L, SEEK_SET)) {
                fprintf(stderr, "Failed to seek to beginning of file %s\n", file_path);
            }

            /*
             * Allocate buffer to correct size to hold all characers in the file,
             * plus a null terminating byte
             */
            size_t len_with_null = (size_t)len + 1;
            char * buf = malloc(sizeof(char) * len_with_null);

            /* Extract text from file and pass to uinput to type */
            fgets(buf, (int)len_with_null, fd);
            if (type_text(buf)) {
                return 1;
            }

            /* Free up buffer memory */
            free(buf);

            if (fclose(fd)) {
                fprintf(stderr, "Failed to close file %s\n", file_path);
            }
		}

        /* Free up filepath memory */
        free(file_path);
        return 0;
	}

    /* No file parameter passed in, so text to type is in remaining args */
    int extra_args = argc - optind;
    if (!extra_args) {
        fprintf(stderr, "Not enough args!\n");
        return type_print_usage();
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

    /* Initialise to null bytes */
    memset(buf, '\0', len+1);
    optind -= extra_args;

    /* Concatenate args into buffer */
    for (; optind != argc; ++optind) {
        strcat(buf, argv[optind]);
    }

    /* Emulate keyboard input of buffer characters */
    if (type_text(buf)) {
        return 1;
    }

    /* Free up buffer memory */
    free(buf);

	return 0;
}
