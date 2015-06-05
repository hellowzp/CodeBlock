#include "FireService.h"
#include <iostream>

FireService::FireService()
{
    //ctor
}

FireService::~FireService()
{
    //dtor
}

void FireService::performService() {
    std::cout << "FireService performed!" << std::endl;
}
