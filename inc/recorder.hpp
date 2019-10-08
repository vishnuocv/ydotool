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

#ifndef YDOTOOL_TOOL_RECORD_HPP
#define YDOTOOL_TOOL_RECORD_HPP

// Local includes
#include "utils.hpp"
// External libs
#include <uInputPlus/uInput.hpp>

struct file_header {
    char magic[4];
    uint32_t crc32;
    uint64_t size;
    uint64_t feature_mask;
} __attribute__((__packed__));

struct data_chunk {
    uint64_t delay[2];
    uint16_t ev_type;
    uint16_t ev_code;
    int32_t ev_value;
} __attribute__((__packed__));

void do_record(const std::vector<std::string> & __devices);
void do_replay(const uInputPlus::uInput * uInputContext);
void do_display();
std::vector<std::string> find_all_devices();
void recorder_help(const char * argv_0);
int recorder_run(int argc, const char ** argv, const uInputPlus::uInput * uInputContext);

#endif //YDOTOOL_TOOL_RECORD_HPP
