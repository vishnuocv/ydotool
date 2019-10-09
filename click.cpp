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
// C system includes
#include <unistd.h>

void click_help(){
	std::cerr << "Usage: click [--delay <ms>] <button>\n"
		<< "  --help                Show this help.\n"
		<< "  --delay ms            Delay time before start clicking. Default 100ms.\n"
		<< "  button                1: left 2: right 3: middle" << std::endl;
}

int click_run(int argc, char ** argv) {
	int time_delay = 100;
    int opt = 0;

    // TODO: Add long options --help and --delay
    while( ( opt = getopt( argc, argv, "hd:" ) ) != -1 )
    {
        switch( opt )
        {
            case 'd':
                time_delay = atoi( optarg );
                if ( errno == ERANGE )
                {
                    std::cerr << "Failed to parse delay argument parameter" << std::endl;
                    click_help();
                    return -1;
                }
                break;
            case 'h':
            case '?':
                click_help();
                return -1;
        }
    }

    if ( argc - optind != 1 )
    {
        if ( argc - optind > 1 )
        {
            std::cerr << "Too many arguments!" << std::endl;
        }
        else
        {
            std::cerr << "Not enough arguments!" << std::endl;
        }
        click_help();
        return -1;
    }

    int button = atoi( argv[optind] );
    if ( errno == ERANGE )
    {
        std::cerr << "Failed to parse button argument!" << std::endl;
        click_help();
        return -1;
    }

    std::cout << "button = " << button << std::endl;

	int keycode = BTN_LEFT;

	switch (button)
    {
        case 1:
            break;
		case 2:
			keycode = BTN_RIGHT;
			break;
		case 3:
			keycode = BTN_MIDDLE;
			break;
		default:
            std::cerr << "Invalid button argument!" << std::endl;
            click_help();
            return -1;
	}

	if (time_delay)
		usleep(time_delay * 1000);

    const uInputPlus::uInput * uInputContext = ydotool_get_context();

    uInputContext->SendKey(keycode, 1);
    uInputContext->SendKey(keycode, 0);

	return argc;
}
