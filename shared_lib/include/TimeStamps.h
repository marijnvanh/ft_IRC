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

		auto GetTimeStamps() const -> std::vector<std::pair<time_t, std::string>> {
			return (this->timeStamps_);
		}

		friend auto operator<<(std::ostream& os, const TimeStamps& ts) -> std::ostream&;
	
	private:
	
		std::vector<std::pair<time_t, std::string>> timeStamps_;
	
	};
}

#endif // __TIME_STAMP_MANAGER_H__