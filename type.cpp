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
// C system includes
#include <getopt.h>
// External libs
#include <evdevPlus/evdevPlus.hpp>

int time_keydelay = 12;

void type_help(){
	std::cerr <<  "Usage: type [--delay milliseconds] [--key-delay milliseconds] [--args N]"
		"[--file <filepath>] <things to type>\n"
		<< "  --help                    Show this help.\n"
		<< "  --delay milliseconds      Delay time before start typing.\n"
		<< "  --key-delay milliseconds  Delay time between keystrokes. Default 12ms.\n"
		<< "  --file filepath           Specify a file, the contents of which will be be typed as if passed as"
				"an argument. The filepath may also be '-' to read from stdin." << std::endl;
}

int type_text(const std::string & text) {
	int pos = 0;

	for (auto & c : text) {
		int isUpper = 0;
		int key_code;
		auto itk = evdevPlus::Table_LowerKeys.find(c);

		if (itk != evdevPlus::Table_LowerKeys.end()) {
			key_code = itk->second;
		} else {
			auto itku = evdevPlus::Table_UpperKeys.find(c);
			if (itku != evdevPlus::Table_UpperKeys.end()) {
				isUpper = 1;
				key_code = itku->second;
			} else {
				return -(pos+1);
			}
		}

		int sleep_time;
        const uInputPlus::uInput * uInputContext = ydotool_get_context();

		if (isUpper) {
			sleep_time = 250 * time_keydelay;
			uInputContext->SendKey(KEY_LEFTSHIFT, 1);
			usleep(sleep_time);
		} else {
			sleep_time = 500 * time_keydelay;
		}

		uInputContext->SendKey(key_code, 1);
		usleep(sleep_time);
		uInputContext->SendKey(key_code, 0);
		usleep(sleep_time);

		if (isUpper) {
			uInputContext->SendKey(KEY_LEFTSHIFT, 0);
			usleep(sleep_time);
		}
	}

	return pos;
}

int type_run(int argc, char ** argv) {
	int time_delay = 100;
	std::string file_path;
    int opt = 0;

    typedef enum {
        opt_help,
        opt_delay,
        opt_key_delay,
        opt_file
    } optlist_t;

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
                file_path = strtoul(optarg, NULL, 10);
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
        std::cerr << "Not enough args!" << std::endl;
        type_help();
        return -1;
    }

	int fd = -1;

	if (!file_path.empty()) {
		if (file_path == "-") {
			fd = STDIN_FILENO;
			fprintf(stderr, "ydotool: type: reading from stdin\n");
		} else {
			fd = open(file_path.c_str(), O_RDONLY);

			if (fd == -1) {
				fprintf(stderr, "ydotool: type: error: failed to open %s: %s\n", file_path.c_str(),
					strerror(errno));
				return 2;
			}
		}
	}

	if (time_delay)
		usleep(time_delay * 1000);

	if (fd >= 0) {
		std::string buf;
		buf.resize(128);

		ssize_t rc;
		while ((rc = read(fd, &buf[0], 128))) {
			if (rc > 0) {
				buf.resize(rc);
				type_text(buf);
				buf.resize(128);
			} else if (rc < 0) {
				fprintf(stderr, "ydotool: type: error: read %s failed: %s\n", file_path.c_str(), strerror(errno));
				return 2;
			}
		}

		close(fd);
	} else {
        std::string txt = "";
        for (; optind != argc; ++optind) {
            txt += argv[optind];
        }
        type_text(txt);
	}

	return argc;
}
