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
	auto timeStamp = std::make_pair(time(NULL), content);

	timeStamps_.push_back(timeStamp);
}

auto TimeStamps::PrintAll() -> void
{
	std::cout << *this;
}

namespace IRC {
	auto operator<<(std::ostream& os, const TimeStamps& ts) -> std::ostream&
	{
		auto it = ts.timeStamps_.begin();

		for (; it != ts.timeStamps_.end(); ++it)
		{
			std::tm * ptm = std::localtime(&it->first);
			char buffer[32];

			std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);

			os << buffer << " - " << it->second << std::endl;
		}
		return os;
	}
}
