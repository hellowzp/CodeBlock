#include "SmokeSensor.h"

SmokeSensor::SmokeSensor()
{
    //ctor
}

SmokeSensor::~SmokeSensor()
{
    //dtor
}

SmokeSensor::SmokeSensor(const SmokeSensor& other)
{
    //copy ctor
}

SmokeSensor& SmokeSensor::operator=(const SmokeSensor& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}
