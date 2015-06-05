#include "COSensor.h"
#include <iostream>

COSensor::COSensor(const std::string& _id, const std::string& vn, float con) :
    SensorComponent(_id,vn), concentration(con)
{
    std::cout << "constructing COSensor.." << std::endl;
}

COSensor::~COSensor(){
    std::cout << "deconstructing COSensor of id " << id << std::endl;
}

COSensor::COSensor(const COSensor& cos):
    SensorComponent(cos.id,cos.vendorName,cos.services), concentration(cos.concentration)
{
    std::cout << "copy constructor of COSensor.." << std::endl;
    isActivated = cos.isActivated;
    parent = cos.parent;
}

void COSensor::describe() const {
    std::cout<< "CO sensor " << id << "\n";
    SensorComponent::describe();
    std::cout<< "value: " << concentration << std::endl;
}
