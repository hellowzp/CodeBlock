#ifndef COSENSOR_H
#define COSENSOR_H

#include "SensorComponent.h"

class COSensor : public SensorComponent {


public:
    COSensor(EmergencyService* _svc);

	~COSensor();

	void alarm();

	void activate();

	void deactivate();
};

#endif // COSENSOR_H

