#pragma once

#include "../Scheduler.h"
#include <string>

class IDataManager
{
public:
	virtual void registerUpdates(Scheduler &scheduler) = 0;
	virtual void deregisterUpdates(Scheduler &scheduler) = 0;

	virtual void update() = 0;

	virtual std::string getSystemName() = 0;

	
};

