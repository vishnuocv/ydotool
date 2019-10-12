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

// Local Includes
#include "key.h"
// C++ system Includes
#include <sstream>
extern "C" {
#include <getopt.h>
#include "uinput.h"
}
// External libs
#include <evdevPlus/evdevPlus.hpp>

void key_help() {
	std::cerr << "Usage: key [--delay <ms>] [--key-delay <ms>] [--repeat <times>] [--repeat-delay <ms>] <key sequence> ...\n"
		<< "  --help                Show this help.\n"
		<< "  --delay ms            Delay time before start pressing keys. Default 100ms.\n"
		<< "  --key-delay ms        Delay time between keystrokes. Default 12ms.\n"
		<< "  --repeat times        Times to repeat the key sequence.\n"
		<< "  --repeat-delay ms     Delay time between repetitions. Default 0ms.\n"
		<< "\n"
		<< "Each key sequence can be any number of modifiers and keys, separated by plus (+)\n"
		<< "For example: alt+r Alt+F4 CTRL+alt+f3 aLT+1+2+3 ctrl+Backspace \n"
		<< "\n"
		<< "Since we are emulating keyboard input, combination like Shift+# is invalid.\n"
		<< "Because typing a `#' involves pressing Shift and 3." << std::endl;
}

std::vector<std::string> explode_string(const std::string & str, char delim) {
	std::vector<std::string> result;
	std::istringstream iss(str);

	for (std::string token; std::getline(iss, token, delim); )
		result.push_back(std::move(token));

	return result;
}

static std::vector<std::string> split_keys(const std::string & keys) {
	if (!strchr(keys.c_str(), '+')) {
		return {keys};
	}

	return explode_string(keys, '+');
}

static std::vector<int> keys_to_codes(const std::string & ks) {
	auto list_keystrokes = split_keys(ks);

	std::vector<int> list_keycodes;

	for (auto & it : list_keystrokes) {
		for (auto & itc : it) {
			if (islower(itc))
				itc = toupper(itc);
		}

		auto t_kms = evdevPlus::Table_ModifierKeys.find(it);
		if (t_kms != evdevPlus::Table_ModifierKeys.end()) {
			list_keycodes.push_back(t_kms->second);
			continue;
		}

		auto t_ks = evdevPlus::Table_FunctionKeys.find(it);
		if (t_ks != evdevPlus::Table_FunctionKeys.end()) {
			list_keycodes.push_back(t_ks->second);
		} else {
			auto t_kts = evdevPlus::Table_LowerKeys.find(tolower(it[0]));

			if (t_kts != evdevPlus::Table_LowerKeys.end()) {
				list_keycodes.push_back(t_kts->second);
			} else {
				throw std::invalid_argument("no matching keycode");
			}
		}
	}

	return list_keycodes;
}

int key_emit_codes(long key_delay, const std::vector<std::vector<int>> & list_keycodes) {
	auto sleep_time = (uint)(key_delay * 1000 / (list_keycodes.size() * 2));

	for (auto & it : list_keycodes) {
		for (auto & it_m : it) {
            uinput_send_key(it_m, 1);
			usleep(sleep_time);
		}

		for (auto i = it.size(); i-- > 0;) {
            uinput_send_key(it[i], 0);
			usleep(sleep_time);
		}
	}

	return 0;
}

int key_run(int argc, char ** argv) {
	int time_delay = 100;
	int repeats = 1;
    int opt = 0;

    typedef enum {
        opt_help,
        opt_delay,
        opt_repeat,
    } optlist_t;

    static struct option long_options[] = {
        { "help",         no_argument,       NULL, opt_help         },
        { "delay",        required_argument, NULL, opt_delay        },
        { "repeat",       required_argument, NULL, opt_repeat       },
    };

    while((opt = getopt_long_only(argc, argv, "hd:r:", long_options, NULL)) != -1 )
    {
        switch (opt)
        {
            case 'd':
            case opt_delay:
                time_delay = strtoul(optarg, NULL, 10);
                break;
            case 'r':
            case opt_repeat:
                repeats = strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case opt_help:
            case '?':
                key_help();
                return -1;
        }
    }

    if (argc == optind) {
        std::cerr << "Not enough args!" << std::endl;
        key_help();
        return -1;
    }

	if (time_delay) {
		usleep(time_delay * 1000);
    }

	std::vector<std::vector<int>> keycodes;

    for (; argc != optind; optind++) {
        keycodes.emplace_back( keys_to_codes(argv[optind]) );
    }

	while (repeats--) {
		key_emit_codes(time_delay, keycodes);
	}

	return argc;
}
