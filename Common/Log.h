#ifndef LOG_HPP_
#define LOG_HPP_

#include <cstdlib>

#include <limits>

#include <string>
#include <iostream>
#include <sstream>

class Log {
public:
	static void setDebugMode(bool value)
	{
		Log::debug = value;
	}

	static void notice(std::string msg)
	{
		if(Log::debug)
		{
			std::cout << "[" << Log::get_time_string() << "] ";
			std::cout << "LOG: ";
			std::cout << msg << "." << std::endl;
		}
	}

	static void warning(std::string warning) {
		if(Log::debug)
		{
			std::cout << "[" << get_time_string() << "] ";
			std::cout << "WARNING: ";
			std::cout << warning << "." << std::endl;
		}
	}

	static void exit_error(std::string error)
	{
		std::cout << std::endl << "[" << Log::get_time_string() << "] ";
		std::cout << "ERROR: ";
		std::cout << error << "." << std::endl;
		std::cout << std::endl;
		waitEnter();
		std::exit(0);
	}

	static void waitEnter()
	{
	  std::cout << "Press ENTER to continue... " << std::flush;
	  std::cin.ignore( std::numeric_limits< std::streamsize > ::max(), '\n' );
	}

	template<typename T>
	static std::string to_string(const T & Value)
	{
		std::ostringstream oss;
		oss << Value;
		return oss.str();
	}

private:
	static bool debug;

	static std::string get_time_string()
	{
		std::ostringstream oss;
		struct tm *now = NULL;
		time_t time_value = 0;
		time_value = time(NULL);
		now = localtime(&time_value);
		if(now->tm_hour < 10) {
			oss << "0" << now->tm_hour << ":";
		}
		else {
			oss << now->tm_hour << ":";
		}
		if(now->tm_min < 10) {
			oss << "0" << now->tm_min << ":";
		}
		else {
			oss << now->tm_min << ":";
		}
		if(now->tm_sec < 10) {
			oss << "0" << now->tm_sec;
		}
		else {
			oss << now->tm_sec;
		}
		return oss.str();
	}

	Log(){}
};

#endif
