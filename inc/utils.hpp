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

#ifndef YDOTOOL_UTILS_HPP
#define YDOTOOL_UTILS_HPP

// C system includes
#include <dirent.h>
// C++ system includes
#include <functional>

void timespec_diff(struct timespec * start, struct timespec * stop, struct timespec * result);
void dir_foreach(const std::string & path, const std::function<int(const std::string &, struct dirent *)> & callback, bool recursive=false);
uint32_t crc32(const void * buf, size_t len);

#endif //YDOTOOL_UTILS_HPP
