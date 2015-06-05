#include "SensorComponent.h"

SensorComponent::SensorComponent(EmergencyService* _svc):
    service(_svc)
{

}

SensorComponent::~SensorComponent() {
    delete service;
}

EmergencyService* SensorComponent::getService() {
	return service;
}

void SensorComponent::setService(EmergencyService* svc) {
	this->service = svc;
}

void SensorComponent::alarm() {
    this->service->performService();
}

void SensorComponent::activate() {
	isActivated = true;
}

void SensorComponent::deactivate() {
	isActivated = false;
}
