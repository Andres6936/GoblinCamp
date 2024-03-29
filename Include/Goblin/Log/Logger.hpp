/* Copyright 2010-2011 Ilkka Halila
This file is part of Goblin Camp.

Goblin Camp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Goblin Camp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Goblin Camp. If not, see <http://www.gnu.org/licenses/>.*/
#pragma once

// Reworked logging module.
// Less verbose in usage (uses macros, though) -- LOG(foo), LOG(foo << bar),
// with more automatic formatting (file, line, function) and no more
// explicit flushing.

#include <fstream>
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>

namespace Logger {
	typedef boost::iostreams::tee_device <std::ostream, std::ofstream> TeeDevice;
	typedef boost::iostreams::stream<TeeDevice> TeeStream;

	extern TeeStream log;
	extern std::ofstream log_file;
	
	void OpenLogFile(const std::string&);
	void CloseLogFile();
	
	TeeStream& Prefix(const char* = nullptr, int = 0, const char* = nullptr);
	const char* Suffix();
}

#define LOG_FUNC(x, func) Logger::Prefix(__FILE__, __LINE__, func) << x << Logger::Suffix()
#define LOG(x) LOG_FUNC(x, __FUNCTION__)
