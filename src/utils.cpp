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
#include "utils.hpp"
// C++ system includes
#include <cstring>
#include <system_error>
// C system includes
#include <dirent.h>
// External libs
#include <boost/crc.hpp>

void ydotool::Utils::timespec_diff(struct timespec * start, struct timespec * stop, struct timespec * result) {
	if ((stop->tv_nsec - start->tv_nsec) < 0) {
		result->tv_sec = stop->tv_sec - start->tv_sec - 1;
		result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
	} else {
		result->tv_sec = stop->tv_sec - start->tv_sec;
		result->tv_nsec = stop->tv_nsec - start->tv_nsec;
	}
}

void ydotool::Utils::dir_foreach(const std::string & path, const std::function<int(const std::string &, struct dirent *)> & callback, bool recursive) {
	DIR * dir;
	struct dirent * ent;

	if ((dir = opendir(path.c_str()))) {
		int dotdot_flag = 0;

		while ((ent = readdir(dir))) {

			if (dotdot_flag != 2) {
				if (strcmp(ent->d_name, ".") == 0) {
					dotdot_flag++;
					continue;
				}
				if (strcmp(ent->d_name, "..") == 0) {
					dotdot_flag++;
					continue;
				}
			}

			int rc = callback(path, ent);

			if (rc == 1)
				continue;
			else if (rc == 2)
				break;

			if (recursive) {
				if (ent->d_type == DT_DIR)
					dir_foreach(path + "/" + std::string(ent->d_name), callback, recursive);
			}

		}

		closedir(dir);
	} else {
		throw std::system_error(errno, std::system_category(), strerror(errno));
	}
}

uint32_t ydotool::Utils::crc32(const void *buf, size_t len) {
	boost::crc_32_type result;
	result.process_bytes(buf, len);
	return result.checksum();
}
