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

#ifndef YDOTOOL_TOOL_TYPE_HPP
#define YDOTOOL_TOOL_TYPE_HPP

// Local includes
#include "tool.hpp"

namespace ydotool {
	namespace Tools {
		class Type : public Tool::ToolTemplate {
		public:
			const char * Name() override;
			int Exec(int argc, const char ** argv) override;
			int TypeText(const std::string & text);
			static void * construct();
		};
	}
}

#endif //YDOTOOL_TOOL_TYPE_HPP
