#include <iostream>
#include "TimeStampManager.h"

using namespace IRC;

TimeStampManager::TimeStampManager()
{

}
TimeStampManager::~TimeStampManager()
{
	this->_timeStamps.clear();
}

auto TimeStampManager::Add(TimeStamp timeStamp) -> void
{
	_timeStamps.push_back(timeStamp);
}

auto TimeStampManager::AddNew(std::string content) -> void
{
	_timeStamps.push_back(TimeStamp(content));
}

auto TimeStampManager::PrintAll() -> void
{
	std::vector<TimeStamp>::iterator it = _timeStamps.begin();

	for (; it != _timeStamps.end(); ++it)
	{
		std::cout << it->ToString() << std::endl;
	}
}
