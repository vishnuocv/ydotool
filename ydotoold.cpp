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

// C++ system includes
#include <thread>
#include <iostream>
extern "C" {
/* System includes */
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
/* Local includes */
#include "uinput.h"
}

static int client_handler(int fd) {
	uinput_raw_data buf;

	while (true) {
		int rc = recv(fd, &buf, sizeof(buf), MSG_WAITALL);

		if (rc == sizeof(buf)) {
			uinput_emit(buf.type, buf.code, buf.value);
		} else {
			return 0;
		}
	}
}

int main() {
	const char path_socket[] = "/tmp/.ydotool_socket";
	unlink(path_socket);
	int fd_listener = socket(AF_UNIX, SOCK_STREAM, 0);

	if (fd_listener == -1) {
		std::cerr << "ydotoold: " << "failed to create socket: " << strerror(errno) << "\n";
		abort();
	}

	sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path_socket, sizeof(addr.sun_path)-1);

	if (bind(fd_listener, (struct sockaddr *)&addr, sizeof(addr))) {
		std::cerr << "ydotoold: " << "failed to bind to socket [" << path_socket << "]: " << strerror(errno) << "\n";
		abort();
	}

	if (listen(fd_listener, 16)) {
		std::cerr << "ydotoold: " << "failed to listen on socket [" << path_socket << "]: " << strerror(errno) << "\n";
		abort();
	}

	chmod(path_socket, 0600);
	std::cerr << "ydotoold: " << "listening on socket " << path_socket << "\n";

	while (int fd_client = accept(fd_listener, nullptr, nullptr)) {
		std::cerr << "ydotoold: accepted client\n";

		std::thread thd(client_handler, fd_client);
		thd.detach();
	}

    uinput_destroy();
}
