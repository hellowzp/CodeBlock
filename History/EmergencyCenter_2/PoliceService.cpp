#include "PoliceService.h"
#include <iostream>

//PoliceService::PoliceService() {
//    std::cout << "constructing PoliceService..." << std::endl;
//}

PoliceService::~PoliceService() {
    std::cout << "destructing PoliceService..." << std::endl;
}

void PoliceService::performService() {
    std::cout << "PoliceService performed!" << std::endl;
}
