#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "Component.h"

class ServiceManager {

private:
    Component* comp;

public:
	void activate();

	void deactivate();

};

#endif
