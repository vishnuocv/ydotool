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
// C++ system includes
#include <sstream>
#include <iostream>
#include <vector>

extern "C" {
/* Local includes */
#include "click.h"
#include "mouse.h"
#include "uinput.h"
// System includes
#include <sys/socket.h>
#include <sys/un.h>
}

std::vector<std::string> explode(const std::string & str, char delim) {
	std::vector<std::string> result;
	std::istringstream iss(str);

	for (std::string token; std::getline(iss, token, delim); ) {
		result.push_back(std::move(token));
	}

	return result;
}

int connect_socket(const char * path_socket) {
	int fd_client = socket(AF_UNIX, SOCK_STREAM, 0);

	if (fd_client == -1) {
		std::cerr << "ydotool: " << "failed to create socket: " << strerror(errno) << "\n";
		return -2;
	}

	sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path_socket, sizeof(addr.sun_path)-1);

	if (connect(fd_client, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)))
		return -1;

	return fd_client;
}

int socket_callback(uint16_t type, uint16_t code, int32_t val, void * userp) {
	uinput_raw_data buf {type, code, val};
	int fd = (intptr_t)userp;
	send(fd, &buf, sizeof(buf), 0);

	return 0;
}

int main(int argc, char ** argv) {
	if (argc < 2 || strncmp(argv[1], "-h", 2) == 0 || strncmp(argv[1], "--h", 3) == 0 || strcmp(argv[1], "help") == 0) {
		std::cerr << "Usage: " << argv[0] << " <cmd> <args>\n"
			<< "Available commands:\n"
			<< "	click\n"
			<< "	key\n"
			<< "	mouse\n"
			<< "	recorder\n"
			<< "	type\n";
		exit(1);
	}

	int ret = 0;
	if ( !strcmp(argv[1], "click") ) {
		ret = click_run(argc-1, &argv[1]);
	} else if ( !strcmp(argv[1], "key") ) {
		ret = key_run(argc-1, &argv[1]);
	} else if ( !strcmp(argv[1], "mouse") ) {
		ret = mouse_run(argc-1, &argv[1]);
	} else if ( !strcmp(argv[1], "recorder") ) {
		ret = recorder_run(argc-1, &argv[1]);
	} else if ( !strcmp(argv[1], "type") ) {
		ret = type_run(argc-1, &argv[1]);
	} else {
		std::cerr <<  "ydotool: Unknown option: " << argv[1] << "\n"
			<< "Run 'ydotool help' for a list of arguments" << std::endl;
		ret = 1;
	}

    uinput_destroy();

	return ret;
}
