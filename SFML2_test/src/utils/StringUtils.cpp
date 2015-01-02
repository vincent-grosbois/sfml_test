#include <algorithm>
#include <sstream>
#include <cassert>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include "utils/StringUtils.h"

namespace utils {

	std::string getDirectory(const std::string& fileName) {
		auto found = fileName.find_last_of("/\\");
		return fileName.substr(0,found+1);
	}

	std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty) {
		std::vector<std::string> result;
		if (delim.empty()) {
			result.push_back(s);
			return result;
		}
		std::string::const_iterator substart = s.begin(), subend;
		while (true) {
			subend = search(substart, s.end(), delim.begin(), delim.end());
			std::string temp(substart, subend);
			if (keep_empty || !temp.empty()) {
				result.push_back(temp);
			}
			if (subend == s.end()) {
				break;
			}
			substart = subend + delim.size();
		}
		return result;
	}

	
	template<class T>
	T readFromStr(const std::string& str) {
		T value;
		std::istringstream (str) >> value ;
		return value;
	}

	template<>
	bool readFromStr(const std::string& str) {
		bool value = true;

		if(str == "false" || str == "FALSE" || str == "0") {
			value = false;
		}

		return value;
	}

	template<>
	sf::Vector2i readFromStr(const std::string& str) {
		sf::Vector2i value;

		auto res = split(str, ",", false);

		bool ok = res.size() == 2 && res[0].at(0) == '(' && res[1].at(res[1].size()-1) == ')' ;

		if(!ok) {
			throw std::exception(std::string("Unable to parse " + str + " as a sf::Vect2i").c_str());
		}

		std::istringstream ( res[0].substr(1) ) >> value.x ;
		std::istringstream ( res[1].substr(0, res[1].size()-1) ) >> value.y ;

		return value;
	}

	template<>
	sf::Vector2f readFromStr(const std::string& str) {
		sf::Vector2f value;

		auto res = split(str, ",", false);

		bool ok = res.size() == 2 && res[0].at(0) == '(' && res[1].at(res[1].size()-1) == ')' ;

		if(!ok) {
			throw std::exception(std::string("Unable to parse " + str + " as a sf::Vect2f").c_str());
		}

		std::istringstream ( res[0].substr(1) ) >> value.x ;
		std::istringstream ( res[1].substr(0, res[1].size()-1) ) >> value.y ;

		return value;
	}

	template<>
	sf::Color readFromStr(const std::string& str) {
		sf::Color value;

		auto res = split(str, ",", false);

		bool ok = res.size() == 4 && res[0].at(0) == '(' && res[3].at(res[3].size()-1) == ')' ;

		if(!ok) {
			throw std::exception(std::string("Unable to parse " + str + " as a sf::Color").c_str());
		}

		int temp;
		std::istringstream ( res[0].substr(1) ) >> temp;
		assert(temp >= 0 && temp <= 255);
		value.r = static_cast<sf::Uint8>(temp);

		std::istringstream ( res[1] ) >> temp;
		assert(temp >= 0 && temp <= 255);
		value.g = static_cast<sf::Uint8>(temp);

		std::istringstream ( res[2]) >> temp;
		assert(temp >= 0 && temp <= 255);
		value.b = static_cast<sf::Uint8>(temp);

		std::istringstream ( res[3].substr(0, res[3].size()-1) ) >> temp;
		assert(temp >= 0 && temp <= 255);
		value.a = static_cast<sf::Uint8>(temp);

		return value;
	}

	template<>
	std::string readFromStr(const std::string& str) {
		return str;
	}

	template
	int readFromStr<int>(const std::string& str);

	template
	float readFromStr<float>(const std::string& str);
	
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

	template void splitValue<int>(const std::string& line, const std::string& key, int& result);
	template void splitValue<bool>(const std::string& line, const std::string& key, bool& result);
	template void splitValue<float>(const std::string& line, const std::string& key, float& result);
	template void splitValue<std::string>(const std::string& line, const std::string& key, std::string& result);
	template void splitValue<sf::Vector2f>(const std::string& line, const std::string& key, sf::Vector2f& result);
	template void splitValue<sf::Vector2i>(const std::string& line, const std::string& key, sf::Vector2i& result);
	template void splitValue<sf::Color>(const std::string& line, const std::string& key, sf::Color& result);

};