#pragma once

#include <vector>
#include <string>

namespace utils {

	std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty = true);

	std::string getDirectory(const std::string& fileName);

	template<class T>
	T readFromStr(const std::string& str);

	template<class T>
	void splitValue(const std::string& line, const std::string& key, T& result);

};