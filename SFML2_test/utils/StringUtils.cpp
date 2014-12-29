#include "StringUtils.h"
#include <algorithm>

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
	std::string readFromStr(const std::string& str) {
		return str;
	}

	template
	int readFromStr<int>(const std::string& str);

	template
	float readFromStr<float>(const std::string& str);


};