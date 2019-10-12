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

#ifndef YDOTOOL_TOOLS_HPP
#define YDOTOOL_TOOLS_HPP

extern "C" {
    void click_help();
    int click_run(int argc, char ** argv);
}

void key_help();
int key_run(int argc, char ** argv);

void mouse_help();
int mouse_run(int argc, char ** argv);

void recorder_help();
int recorder_run(int argc, char ** argv);

void type_help();
int type_run(int argc, char ** argv);

#endif //YDOTOOL_TOOLS_HPP
