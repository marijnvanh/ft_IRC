#include "TimeStamps.h"

using namespace IRC;

TimeStamps::TimeStamps()
{

}
TimeStamps::~TimeStamps()
{
	this->timeStamps_.clear();
}

auto TimeStamps::Add(std::string content) -> void
{
	std::pair<time_t, std::string> timeStamp = std::make_pair(time(NULL), content);

	timeStamps_.push_back(timeStamp);
}

auto TimeStamps::PrintAll() -> void
{
	std::vector<std::pair<time_t, std::string>>::iterator it = timeStamps_.begin();

	for (; it != timeStamps_.end(); ++it)
	{
		PrintTimeStamp(*it);
	}
}

auto TimeStamps::PrintTimeStamp(std::pair<time_t, std::string> timeStamp) -> void
{
	std::tm * ptm = std::localtime(&timeStamp.first);
	char buffer[32];

	std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);

	std::cout << buffer << " - " << timeStamp.second << std::endl;	
}
