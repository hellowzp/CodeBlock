#include "SensorComponent.h"
#include <iostream>
#include <algorithm>

SensorComponent::SensorComponent(const std::string& _id, const std::string& vn,
                                 const std::vector<std::shared_ptr<EmergencyService>>& svc):
    Component(_id), vendorName(vn), services(svc)
{
    std::cout << "constructing SensorComponent.." << std::endl;
}

SensorComponent::~SensorComponent() {
    std::cout << "destructing SensorComponent of id " << id << std::endl;
}

SensorComponent::SensorComponent(const SensorComponent& sc):
    Component(sc.id,sc.isActivated,sc.parent),
    vendorName(sc.vendorName), services(sc.services)
{
    std::cout << "copy constructor of SensorComponent.." << std::endl;
}

void SensorComponent::addService(const std::shared_ptr<EmergencyService>& svc) {
    if (std::find(std::begin(services), std::end(services), svc) == std::end(services))
        services.push_back(svc);
}

void SensorComponent::removeService(std::shared_ptr<EmergencyService>& svc) {
    services.erase(std::remove(services.begin(), services.end(), svc), services.end());
}

void SensorComponent::removeAllServices() {
    services.clear();
}

std::vector<std::shared_ptr<EmergencyService>> SensorComponent::getServices() const {
    return services;
}

void SensorComponent::alarm() const{
    if(isActivated) {
        std::cout << "###Alarm & Attention###\n";
        describe();
        for(auto svc : services)
            svc->performService();
    } else {
        std::cout << "Sensor deactivated..." << std::endl;
    }
}

void SensorComponent::alarmService(std::shared_ptr<EmergencyService>& svc) {
    svc->performService();
}

void SensorComponent::activate() {
	isActivated = true;
}

void SensorComponent::deactivate() {
	isActivated = false;
}

void SensorComponent::describe() const {
    std::cout << "Location: ";
    parent->describe();
}
