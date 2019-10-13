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
 * @file mouse.h
 * @author Harry Austen
 * @brief Interface for function for emulating moving your mouse around
 */

#ifndef __MOUSE_H__
#define __MOUSE_H__

/**
 * @brief Moves the move absolutely or relatively by the given x/y coordinates
 * @param argc The number of (remaining) program arguments
 * @param argv Pointer to the (remaining) program arguments
 * @return 0 on success, 1 if error(s)
 */
int mouse_run(int argc, char ** argv);

#endif /* __MOUSE_H__ */
