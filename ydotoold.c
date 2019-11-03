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
 * @file ydotoold.c
 * @author Harry Austen
 * @brief Main entry point to the ydotool daemon program. Run this in the background to speed up the ydotool program commands
 */

/* System includes */
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/* Local includes */
#include "uinput.h"

void * client_handler(void * fdp) {
	struct uinput_raw_data buf;
    int fd = *(int *)fdp;

	for (;;) {
		ssize_t rc = recv(fd, &buf, sizeof(buf), MSG_WAITALL);

		if (rc != sizeof(buf)) {
			break;
		}

		uinput_emit(buf.type, buf.code, buf.value);
	}

    pthread_exit(NULL);
}

int main() {
	const char * path_socket = "/tmp/.ydotool_socket";
	unlink(path_socket);
	int fd_listener = socket(AF_UNIX, SOCK_STREAM, 0);

	if (fd_listener == -1) {
		fprintf(stderr, "ydotoold: failed to create socket: %s\n", strerror(errno));
		return 1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path_socket, sizeof(addr.sun_path)-1);

	if (bind(fd_listener, (struct sockaddr *)&addr, sizeof(addr))) {
		fprintf(stderr, "ydotoold: failed to bind to socket [%s]: %s\n", path_socket, strerror(errno));
		return 1;
	}

	if (listen(fd_listener, 16)) {
		fprintf(stderr, "ydotoold: failed to listen on socket [%s]: %s\n", path_socket, strerror(errno));
		return 1;
	}

	chmod(path_socket, 600);
	printf("ydotoold: listening on socket %s\n", path_socket);

    int fd_client = 0;
	while ((fd_client = accept(fd_listener, NULL, NULL))) {
		printf("ydotoold: accepted client\n");

        pthread_t thd;
        if (pthread_create(&thd, NULL, client_handler, (void *)&fd_client)) {
            fprintf(stderr, "ydotoold: Error creating thread!\n");
            return 1;
        }

        if (pthread_detach(thd)) {
            fprintf(stderr, "ydotoold: Error detaching thread!\n");
            return 1;
        }
	}

    if (uinput_destroy()) {
        return 1;
    }
    return 0;
}
