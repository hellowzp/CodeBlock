#include "SensorComponent.h"
#include <iostream>
#include <algorithm>

SensorComponent::SensorComponent(const std::string& _id, const std::string& vn):
    Component(_id), vendorName(vn)
{
    std::cout << "constructing SensorComponent.." << std::endl;
}

SensorComponent::SensorComponent(const std::string& _id, const std::string& vn, const vSpSVC& svc):
     Component(_id), vendorName(vn), services(svc)
{
    std::cout << "constructing SensorComponent.." << std::endl;
}

SensorComponent::~SensorComponent() {
    std::cout << "destructing SensorComponent..." << std::endl;
}

SensorComponent::SensorComponent(const SensorComponent& sc):
    Component(sc.id),vendorName(sc.vendorName), services(sc.services)
{
    std::cout << "copy constructor of SensorComponent.." << std::endl;
    isActivated = sc.isActivated;
    parent = sc.parent;
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
            if(svc!=std::shared_ptr<EmergencyService>(nullptr))
                svc->performService();
    } else {
        std::cout << "Sensor not activated..." << std::endl;
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
    if(parent) parent->describe();
    else std::cout << "unknown\n";
}

const std::string SensorComponent::getFullName(bool reverse) const {
    if(parent) {
        if(reverse) return (parent->getFullName(true) + " Sensor" + id);
        else return ("Sensor" + id + " " + parent->getFullName());
    } else {
        return "Sensor" + id;
    }
}

std::shared_ptr<Component> SensorComponent::searchComponentByFullName(const std::string& fname){
    std::string fullName = this->getFullName(true);
//    fullName.resize(fullName.length()-1); //remove the last space
    std::cout<< "searching for "  << fname << " in " << fullName << "..\n";

    if(getFullName(true)==fname)
        return std::shared_ptr<Component>{this};
    else
        return {nullptr};
}

std::ostream& operator<<(std::ostream& os, SensorComponent& com) {
    os << "@Sensor" << com.id << "\n";
//    auto parent = com.getParent();
//    if(parent != nullptr ) {
//        os << " " << *(dynamic_cast<GroupContainer*>(parent.get()));
//    } else {
//        os << "\n";
//    }
    return os;
}
