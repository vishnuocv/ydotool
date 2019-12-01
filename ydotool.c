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
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

/* Local includes */
#include "uinput.h"

/**
 * Print usage string to stderr
 * @return 1 (error)
 */
int click_print_usage() {
    fprintf(stderr,
        "Usage: click [--delay <ms>] <button>\n"
        "    --help      Show this help\n"
        "    --delay ms  Delay time before start clicking (default = 100ms)\n"
        "    button      1: left\n"
        "                2: right\n"
        "                3: middle\n");
    return 1;
}

/**
 * @brief Click a particular mouse button once
 * @param argc The (remaining) number of program arguments
 * @param argv Pointer to the (remaining) program arguments
 * @return 0 on success, 1 if error(s)
 */
int click_run(int argc, char ** argv) {
	uint32_t time_delay = 100;
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
                time_delay = (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case opt_help:
            case '?':
                return click_print_usage();
        }
    }

    int extra_args = argc - optind;
    if (extra_args != 1) {
        fprintf(stderr, (extra_args > 1) ? "Too many arguments!\n" : "Not enough arguments!\n");
        return click_print_usage();
    }

    uint16_t button = (uint16_t)strtoul(argv[optind], NULL, 10);
	uint16_t keycode = BTN_LEFT;

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
            return click_print_usage();
	}

	if (time_delay) {
		usleep(time_delay * 1000);
    }

    if (uinput_send_keypress(keycode)) {
        return 1;
    }

	return 0;
}

/**
 * Print usage string to stderr
 * @return 1 (error)
 */
int key_print_usage() {
    fprintf(stderr,
        "Usage: key [--delay <ms>] [--key-delay <ms>] [--repeat <times>] [--repeat-delay <ms>] <key sequence> ...\n"
        "    --help             Show this help\n"
        "    --delay ms         Delay time before start pressing keys (default = 100ms)\n"
        "    --key-delay ms     Delay time between keystrokes (default = 12ms)\n"
        "    --repeat times     Times to repeat the key sequence\n"
        "    --repeat-delay ms  Delay time between repetitions (default = 0ms)\n"
        "Each key sequence can be any number of modifiers and keys, separated by plus (+)\nFor example: alt+r Alt+F4 CTRL+alt+f3 aLT+1+2+3 ctrl+Backspace\n");
    return 1;
}

/**
 * Press all keys, then release all keys
 * @param key_string Sequence of string representations of keys to be pressed together, separated by '+'
 * @return 0 on success, 1 if error(s)
 */
int key_enter_keys(char * key_string) {
    char * ptr = strtok(key_string, "+");
    while (ptr) {
        if (uinput_enter_key(ptr, 1)) {
            return 1;
        }
        ptr = strtok(NULL, "+");
    }
    ptr = strtok(key_string, "+");
    while (ptr) {
        if (uinput_enter_key(ptr, 0)) {
            return 1;
        }
        ptr = strtok(NULL, "+");
    }
    return 0;
}

/**
 * @brief Emulate entering any number of given sequences of keys
 * @param argc Number of (remaining) program arguments
 * @param argv Pointer to the (remaining) program arguments
 * @return 0 on success, 1 if error(s)
 */
int key_run(int argc, char ** argv) {
	uint32_t time_delay = 100;
	uint64_t repeats = 1;
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
                time_delay = (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case 'r':
            case opt_repeat:
                repeats = strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case opt_help:
            case '?':
                return key_print_usage();
        }
    }

    if (argc == optind) {
        fprintf(stderr, "Not enough args!\n");
        return key_print_usage();
    }

	if (time_delay) {
		usleep(time_delay * 1000);
    }

    const int first_arg = optind;

    while (repeats--) {
        for (; argc != optind; optind++) {
            if (key_enter_keys(argv[optind])) {
                return 1;
            }
        }
        optind = first_arg;
    }

	return 0;
}

/**
 * Print usage string to stderr
 * @return 1 (error)
 */
int mouse_print_usage() {
    fprintf(stderr,
        "Usage: mouse [--delay <ms>] <x> <y>\n"
        "    --help      Show this help\n"
        "    --delay ms  Delay time before start moving (default = 100ms)\n");
    return 1;
}

/**
 * @brief Moves the move absolutely or relatively by the given x/y coordinates
 * @param argc The number of (remaining) program arguments
 * @param argv Pointer to the (remaining) program arguments
 * @return 0 on success, 1 if error(s)
 */
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

/**
 * @brief Emulate keyboard input to type the given text using a virtual keyboard device
 * @param argc The number of (remaining) program arguments
 * @param argv Pointer to the (remaining) program arguments
 * @return 0 on success, 1 on error(s)
 */
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
