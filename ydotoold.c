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
#include <signal.h>

/* Local includes */
#include "uinput.h"

/**
 * File decriptor for the socket listener
 */
static int FD_LIST = -1;

/**
 * Function for handling user interruption (Ctrl-C)
 * @param sig The signal received by the program
 */
void sig_handler(int sig) {
    printf("\nReceived %s. Terminating...\n", sys_siglist[sig]);
    uinput_destroy();
    close(FD_LIST);
    exit(0);
}

/**
 * Function for handling a uinput event sent from the main ydotool program via socket
 * @param fdp File descriptor pointer for the open socket
 */
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

/**
 * Main entrypoint to the ydotool daemon program
 * @return 0 on success, 1 if error(s)
 */
int main() {
    /* Setup SIGINT signal handling */
    struct sigaction act;
    act.sa_handler = &sig_handler;
    sigaction(SIGINT, &act, NULL);

    /* Initialise input device */
    if (uinput_init()) {
        return 1;
    }

    /* Create socket */
	const char * path_socket = "/tmp/.ydotool_socket";
	unlink(path_socket);
	FD_LIST = socket(AF_UNIX, SOCK_STREAM, 0);

	if (FD_LIST == -1) {
		fprintf(stderr, "ydotoold: failed to create socket: %s\n", strerror(errno));
		return 1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path_socket, sizeof(addr.sun_path)-1);

	if (bind(FD_LIST, (struct sockaddr *)&addr, sizeof(addr))) {
		fprintf(stderr, "ydotoold: failed to bind to socket [%s]: %s\n", path_socket, strerror(errno));
		return 1;
	}

	if (listen(FD_LIST, 16)) {
		fprintf(stderr, "ydotoold: failed to listen on socket [%s]: %s\n", path_socket, strerror(errno));
		return 1;
	}

    mode_t open_access = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	chmod(path_socket, open_access);
	printf("ydotoold: listening on socket %s\n", path_socket);

    /* Wait for tasks */
    int fd_client = 0;
	while ((fd_client = accept(FD_LIST, NULL, NULL))) {
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

    /* If socket become invalidated, destroy input device and close socket */
    if (uinput_destroy() || close(FD_LIST)) {
        return 1;
    }
    return 0;
}
