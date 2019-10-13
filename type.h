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
 * @file type.h
 * @author Harry Austen
 * @brief Interface for function for emulating typing text
 */

#ifndef __TYPE_H__
#define __TYPE_H__

/**
 * @brief Emulate keyboard input to type the given text using a virtual keyboard device
 * @param argc The number of (remaining) program arguments
 * @param argv Pointer to the (remaining) program arguments
 * @return 0 on success, 1 on error(s)
 */
int type_run(int argc, char ** argv);

#endif /* __TYPE_H__ */
