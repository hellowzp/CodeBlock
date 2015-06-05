#include "Component.h"
#include <iostream>

Component::Component(bool _isActivated):
    isActivated(_isActivated), parent(nullptr)
{
    std::cout<< "constructing Component.." << std::endl;
}

Component::~Component() {
    std::cout<< "destructing Component.." << std::endl;
}

//void Component::setParent(std::shared_ptr<Component> _parent) {
//    parent = _parent;
//}
//void Component::alarm() {}
//
//void Component::activate() {}
//
//void Component::deactivate() {}

