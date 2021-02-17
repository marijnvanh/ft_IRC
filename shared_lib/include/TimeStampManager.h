#ifndef __TIME_STAMP_MANAGER_H__
#define __TIME_STAMP_MANAGER_H__

#include <vector>
#include <string>
#include <time.h>
#include <ctime>

namespace IRC
{
	struct TimeStamp
	{

	public:

		TimeStamp();
		TimeStamp(std::string content) : _stamp(time(NULL)), _content(content) {}
		TimeStamp(time_t stamp, std::string content) : _stamp(stamp), _content(content) {}

		/**
		 * @brief Returns an std::string object with the formatted timestamp + content of the TimeStamp object.
		 * 
		 * @returns std::string object containing timestamp data.
		 */
		auto ToString() -> std::string {
			std::tm * ptm = std::localtime(&_stamp);
			char buffer[32];
			// Format: Mo, 15.06.2009 20:20:00
			std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);

			return (buffer + std::string(" - ") + _content);
		}

	private:
	
		time_t _stamp;
		std::string _content;

	};

	class TimeStampManager
	{
	
	public:
	
		TimeStampManager();
		~TimeStampManager();

		/**
		 * @brief Adds the given TimeStamp object to the list of available timestamps.
		 */
		auto Add(TimeStamp timeStamp) -> void;

		/**
		 * @brief Adds a new TimeStamp object to the list of available timestamps, using the given 'content' and the current time.
		 */
		auto AddNew(std::string content) -> void;

		auto PrintAll() -> void;
	
	private:
	
		std::vector<TimeStamp> _timeStamps;
	
	};
}

#endif // __TIME_STAMP_MANAGER_H__