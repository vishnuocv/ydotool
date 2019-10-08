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
// C++ system includes
#include <iostream>
// External libs
#include <boost/program_options.hpp>

void mouse_help(const char * argv_0){
	std::cerr << "Usage: " << argv_0 << " [--delay <ms>] <x> <y>\n"
			<< "  --help                Show this help.\n"
			<< "  --delay ms            Delay time before start moving. Default 100ms." << std::endl;
}

int mouse_run(int argc, const char ** argv, const uInputPlus::uInput * uInputContext) {
	int time_delay = 100;

	std::vector<std::string> extra_args;

	try {
		boost::program_options::options_description desc("");
		desc.add_options()
			("help", "Show this help")
			("delay", boost::program_options::value<int>())
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
			mouse_help(argv[0]);
			return -1;
		}

		if (vm.count("delay")) {
			time_delay = vm["delay"].as<int>();
			std::cerr << "Delay was set to "
				  << time_delay << " milliseconds.\n";
		}

		if (extra_args.size() != 2) {
			std::cerr << "Which coordinate do you want to move to?\n"
				     "Use `ydotool " << argv[0] << " --help' for help.\n";

			return 1;
		}
	} catch (std::exception &e) {
		std::cerr <<  "ydotool: " << argv[0] << ": error: " << e.what() << std::endl;
		return 2;
	}

	if (time_delay)
		usleep(time_delay * 1000);

	auto x = (int32_t)strtol(extra_args[0].c_str(), nullptr, 10);
	auto y = (int32_t)strtol(extra_args[1].c_str(), nullptr, 10);

	if (!strchr(argv[0], '_')) {
		uInputContext->RelativeMove({-INT32_MAX, -INT32_MAX});
	}

	uInputContext->RelativeMove({x, y});

	return argc;
}
