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
 * @file click.h
 * @author Harry Austen
 * @brief Provides a function for simluating mouse clicks
 */

#ifndef __CLICK_H__
#define __CLICK_H__

/**
 * @brief Click a particular mouse button once
 * @param argc The (remaining) number of program arguments
 * @param argv Pointer to the (remaining) program arguments
 * @return 0 on success, 1 if error(s)
 */
int click_run(int argc, char ** argv);

#endif /* __CLICK_H__ */
