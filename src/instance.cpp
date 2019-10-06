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
#include "instance.hpp"

void ydotool::Instance::Init() {
	Init("ydotool virtual device");
}

void ydotool::Instance::Init(const std::string &device_name) {
    uInputPlus::uInputSetup us(device_name);
	uInputContext = std::make_unique<uInputPlus::uInput>();
	uInputContext->Init({us});
}
