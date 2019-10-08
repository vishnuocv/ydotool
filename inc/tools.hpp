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

// Local includes
#include "utils.hpp"
// External libs
#include <uInputPlus/uInput.hpp>

// Click
void click_help();
int click_run(int argc, const char ** argv, const uInputPlus::uInput * uInputContext);

// Key
void key_help();
int key_run(int argc, const char ** argv, const uInputPlus::uInput * uInputContext);
int key_emit_codes(long key_delay, const std::vector<std::vector<int>> & list_keycodes, const uInputPlus::uInput * uInputContext);

// Mouse
void mouse_help(const char * argv_0);
int mouse_run(int argc, const char ** argv, const uInputPlus::uInput * uInputContext);

// recorder
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

// Type
void type_help();
int type_run(int argc, const char ** argv, const uInputPlus::uInput * uInputContext);
int type_text(const std::string & text, const uInputPlus::uInput * uInputContext);

#endif //YDOTOOL_TOOLS_HPP
