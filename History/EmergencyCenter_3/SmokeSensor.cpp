#include "SmokeSensor.h"
#include <iostream>

SmokeSensor::SmokeSensor(const std::string& _id, const std::string& vn, float _sen):
    SensorComponent(_id,vn), sensitivity(_sen)
{
    std::cout << "constructing SmokeSensor.." << std::endl;
}

SmokeSensor::~SmokeSensor()
{
    std::cout << "destructing SmokeSensor with info\n";
    describe();
}

SmokeSensor::SmokeSensor(const SmokeSensor& other):
    SensorComponent(other.id,other.vendorName,other.services), sensitivity(other.sensitivity)
{
    std::cout << "copy constructor of SmokeSensor.." << std::endl;
    isActivated = other.isActivated;
    parent = other.parent;
}

SmokeSensor& SmokeSensor::operator=(const SmokeSensor& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void SmokeSensor::describe() const {
    std::cout<< "@SmokeSensor " << id << "vendor name: " << vendorName << "\n";
    SensorComponent::describe();
    std::cout<< "sensibility: " << sensitivity << std::endl;
}
