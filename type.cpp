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
#include "tools.hpp"
// External libs
#include <boost/program_options.hpp>
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
	std::vector<std::string> extra_args;

	try {
		boost::program_options::options_description desc("");
		desc.add_options()
			("help", "Show this help")
			("delay", boost::program_options::value<int>())
			("key-delay", boost::program_options::value<int>())
			("file", boost::program_options::value<std::string>())
			("extra-args", boost::program_options::value(&extra_args));


		boost::program_options::positional_options_description p;
		p.add("extra-args", -1);

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
			options(desc).
			positional(p).
			run(), vm);
		boost::program_options::notify(vm);

		if (vm.count("help")) {
			type_help();
			return -1;
		}

		if (vm.count("delay")) {
			time_delay = vm["delay"].as<int>();
			std::cerr << "Delay was set to "
				  << time_delay << " milliseconds.\n";
		}

		if (vm.count("key-delay")) {
			time_keydelay = vm["key-delay"].as<int>();
			std::cerr << "Key delay was set to "
				  << time_keydelay << " milliseconds.\n";
		}

		if (vm.count("file")) {
			file_path = vm["file"].as<std::string>();
			std::cerr << "File path was set to "
				  << file_path << ".\n";
		} else {
			if (extra_args.empty()) {
				std::cerr << "What do you want to type?\n"
					     "Use `ydotool type --help' for help.\n";

				return 1;
			}
		}
	} catch (std::exception & e) {
		fprintf(stderr, "ydotool: type: error: %s\n", e.what());
		return 2;
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
		for (auto &txt : extra_args) {
			type_text(txt);
		}
	}

	return argc;
}
