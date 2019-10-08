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

#ifndef YDOTOOL_TOOL_MOUSEMOVE_HPP
#define YDOTOOL_TOOL_MOUSEMOVE_HPP

// External libs
#include <uInputPlus/uInput.hpp>

void mouse_help(const char * argv_0);
int mouse_run(int argc, const char ** argv, const uInputPlus::uInput * uInputContext);

#endif //YDOTOOL_TOOL_MOUSEMOVE_HPP
