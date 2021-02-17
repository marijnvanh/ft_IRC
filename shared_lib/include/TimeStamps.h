#ifndef __TIME_STAMP_MANAGER_H__
#define __TIME_STAMP_MANAGER_H__

#include <ctime>
#include <vector>
#include <string>
#include <time.h>
#include <iostream>

namespace IRC
{
	class TimeStamps
	{
	
	public:
	
		TimeStamps();
		~TimeStamps();

		/**
		 * @brief Adds the given TimeStamp object to the list of available timestamps.
		 */
		auto Add(std::string content) -> void;

		auto PrintAll() -> void;
	
	private:
	
		std::vector<std::pair<time_t, std::string>> timeStamps_;

		/**
		 * @brief Prints the given timestamp pair with the format ` Mo, 15.06.2009 20:20:00 - Content `.
		 */
		auto PrintTimeStamp(std::pair<time_t, std::string> timeStamp) -> void;
	
	};
}

#endif // __TIME_STAMP_MANAGER_H__