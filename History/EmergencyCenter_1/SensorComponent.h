#ifndef SENSORCOMPONENT_H
#define SENSORCOMPONENT_H

#include "Component.h"
#include "EmergencyService.h"
#include <string>

class SensorComponent : public Component {

protected:
	EmergencyService* service;

public:
    SensorComponent(EmergencyService* _svc = NULL);

    ~SensorComponent();

	EmergencyService* getService();

	void setService(EmergencyService* svc);

	void alarm();

	void activate();

	void deactivate();
};

#endif // SENSORCOMPONENT_H

