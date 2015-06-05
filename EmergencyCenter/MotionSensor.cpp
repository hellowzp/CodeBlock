#include "MotionSensor.h"
#include <iostream>

MotionSensor::MotionSensor(const std::string& _id, const std::string& vn,
                            const float _min, const float _max):
    SensorComponent(_id,vn), min(_min), max(_max)
{
    std::cout << "constructing MotionSensor.." << std::endl;
}

MotionSensor::~MotionSensor()
{
    std::cout << "destructing MotionSensor with info\n";
    describe();
}

MotionSensor::MotionSensor(const MotionSensor& other):
    SensorComponent(other.id,other.vendorName,other.services),
    min(other.min), max(other.max)
{
    std::cout << "copy constructor of MotionSensor.." << std::endl;
    isActivated = other.isActivated;
    parent = other.parent;
}

MotionSensor& MotionSensor::operator=(const MotionSensor& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void MotionSensor::describe() const {
    std::cout<< "@MotionSensor " << id << "\nvendor name: " << vendorName << "\n";
    SensorComponent::describe();
    std::cout<< "range: " << min << "-" << max << std::endl;
}

