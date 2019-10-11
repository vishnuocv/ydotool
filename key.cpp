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
#include "tools.hpp"
// C++ system Includes
#include <sstream>
// C system includes
#include <getopt.h>
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

int key_emit_codes(long key_delay, const std::vector<std::vector<int>> & list_keycodes, const uInputPlus::uInput * uInputContext) {
	auto sleep_time = (uint)(key_delay * 1000 / (list_keycodes.size() * 2));

	for (auto & it : list_keycodes) {
		for (auto & it_m : it) {
			uInputContext->SendKey(it_m, 1);
			usleep(sleep_time);
		}

		for (auto i = it.size(); i-- > 0;) {
			uInputContext->SendKey(it[i], 0);
			usleep(sleep_time);
		}
	}

	return 0;
}

int key_run(int argc, char ** argv) {
	int time_delay = 100;
	int time_keydelay = 12;
	int time_repdelay = 0;

	int repeats = 1;
    int opt = 0;

    typedef enum {
        opt_help,
        opt_delay,
        opt_key_delay,
        opt_repeat,
        opt_repeat_delay
    } optlist_t;

    static struct option long_options[] = {
        { "help",         no_argument,       NULL, opt_help         },
        { "delay",        required_argument, NULL, opt_delay        },
        { "key-delay",    required_argument, NULL, opt_key_delay    },
        { "repeat",       required_argument, NULL, opt_repeat       },
        { "repeat-delay", required_argument, NULL, opt_repeat_delay }
    };

    while((opt = getopt_long_only(argc, argv, "hd:k:r:y:", long_options, NULL)) != -1 )
    {
        switch (opt)
        {
            case 'd':
            case opt_delay:
                time_delay = strtoul(optarg, NULL, 10);
                break;
            case 'k':
            case opt_key_delay:
                time_keydelay = strtoul(optarg, NULL, 10);
            case 'r':
            case opt_repeat:
                repeats = strtoul(optarg, NULL, 10);
            case 'y':
            case opt_repeat_delay:
                time_repdelay = strtoul(optarg, NULL, 10);
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

    const uInputPlus::uInput * uInputContext = ydotool_get_context();

	while (repeats--) {
		key_emit_codes(time_delay, keycodes, uInputContext);
	}

	return argc;
}
