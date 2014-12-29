#pragma once

#include <vector>
#include <sstream>
#include <cassert>

#include <SFML/System/Vector2.hpp>

namespace utils {

	std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty = true);

	std::string getDirectory(const std::string& fileName);

	template<class T>
	T readFromStr(const std::string& str);

	template<class T>
	void splitValue(const std::string& line, const std::string& key, T& result) {
		auto split_result = split(line, ":", true);

		if( split_result.size() != 2) {
			throw std::exception(std::string("Could not split " + line).c_str());
		}
		if(split_result[0] != key) {
			throw std::exception(std::string("Expected token " + key + " , received " + split_result[0]).c_str());
		}

		result = readFromStr<T>(split_result[1]);
	}

};