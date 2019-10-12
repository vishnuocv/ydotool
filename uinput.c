/* Local includes */
#include "uinput.h"
/* System includes */
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define CHECK(X) if (X == -1) { fprintf( stderr, "ERROR (%s:%d) -- %s\n", __FILE__, __LINE__, strerror(errno) ); exit(-1); }

int FD = -1;

int KEYCODES[] = {
    BTN_LEFT,
    BTN_RIGHT,
    BTN_MIDDLE
};

int EVCODES[] = {
    EV_KEY,
    EV_REL,
    EV_ABS
};

/* Initialise the input device */
void uinput_init() {
    if (access("/dev/uinput", W_OK)) {
        fprintf(stderr, "Do not have access to write to /dev/uinput!\nTry running as root\n");
        exit(-1);
    }
    CHECK( (FD = open("/dev/uinput", O_WRONLY|O_NONBLOCK)) );

    /* Events/Keys setup */
    for (int i = 0; i != sizeof(KEYCODES)/sizeof(KEYCODES[0]); ++i) {
        CHECK( ioctl(FD, UI_SET_KEYBIT, KEYCODES[i]) );
    }
    for (int i = 0; i != sizeof(EVCODES)/sizeof(EVCODES[0]); ++i) {
        CHECK( ioctl(FD, UI_SET_EVBIT, EVCODES[i]) );
    }

    /* uinput device setup */
    struct uinput_setup usetup = {
        {
            BUS_USB,
            0x1234,
            0x5678,
            0
        },
        "ydotool virtual device",
        0
    };

    CHECK( ioctl(FD, UI_DEV_SETUP, &usetup) );
    CHECK( ioctl(FD, UI_DEV_CREATE) );
}

/* Delete the input device */
void uinput_destroy() {
    if (FD != -1) {
        CHECK( ioctl(FD, UI_DEV_DESTROY) );
        close(FD);
        FD = -1;
    }
}

/* Trigger an input event */
void uinput_emit(uint16_t type, uint16_t code, int32_t value) {
    struct input_event ie = {
        /* Ignore timestamp values */
        {0,0},
        type,
        code,
        value
    };

    if (FD == -1) {
        uinput_init();
    }

    CHECK( write(FD, &ie, sizeof(ie)) );

    /* Allow processing time for uinput before sending next event */
    usleep( 50 );
}

/* Single key event and report */
void uinput_send_key(uint16_t code, int32_t value) {
    uinput_emit(EV_KEY, code,       value);
    uinput_emit(EV_SYN, SYN_REPORT, 0    );
}

/* Simulate a quick key press */
void uinput_send_keypress(uint16_t code) {
    /* send press */
    uinput_send_key(code, 1);
    /* send release */
    uinput_send_key(code, 0);
}

/* Move the cursor to a given (x,y) position */
void uinput_move_mouse(int32_t x, int32_t y) {
    uinput_emit(EV_ABS, ABS_X, x);
    uinput_emit(EV_ABS, ABS_Y, y);
    uinput_emit(EV_SYN, SYN_REPORT, 0);
}

/* Move the cursor a given (x,y) relative to the current position */
void uinput_relative_move_mouse(int32_t x, int32_t y) {
    if (x) {
        uinput_emit(EV_REL, REL_X, x);
    }
    if (y) {
        uinput_emit(EV_REL, REL_Y, y);
    }
    uinput_emit(EV_SYN, SYN_REPORT, 0);
}
