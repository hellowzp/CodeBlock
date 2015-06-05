#include "EvacuationService.h"
#include <iostream>

EvacuationService::EvacuationService()
{
    //ctor
}

EvacuationService::~EvacuationService()
{
    //dtor
}

EvacuationService::EvacuationService(const EvacuationService& other)
{
    //copy ctor
}

EvacuationService& EvacuationService::operator=(const EvacuationService& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void EvacuationService::performService() {
    std::cout << "EvacuationService performed!" << std::endl;
}
