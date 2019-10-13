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
 * @file key.h
 * @author Harry Austen
 * @brief Interface for function to emulate entering key sequences
 */

#ifndef __KEY_H__
#define __KEY_H__

/**
 * @brief Emulate entering any number of given sequences of keys
 * @param argc Number of (remaining) program arguments
 * @param argv Pointer to the (remaining) program arguments
 * @return 0 on success, 1 if error(s)
 */
int key_run(int argc, char ** argv);

#endif /* __KEY_H__ */
