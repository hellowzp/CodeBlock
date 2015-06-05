#include "COSensor.h"

COSensor::COSensor(EmergencyService* _svc) :
    SensorComponent(_svc)
{

}

COSensor::~COSensor(){
    delete service;
}

void COSensor::alarm() {
	service->performService();
}

void COSensor::activate() {
	isActivated = true;
}

void COSensor::deactivate() {
	isActivated = false;
}
