#include "Component.h"
#include <iostream>

Component::Component(const std::string& _id):
    id(_id), isActivated(true), parent(nullptr)
{
    std::cout<< "constructing Component.." << std::endl;
}

//Component::Component(const std::string& _id, bool _isAct, const std::shared_ptr<Component>& _parent):
//    id(_id), isActivated(_isAct), parent(_parent)
//{
//    std::cout<< "constructing Component.." << std::endl;
//}

Component::~Component() {
    std::cout<< "destructing Component.." << std::endl;
}

Component::Component(const Component& comp):
    id(comp.id), isActivated(comp.isActivated),  parent(comp.parent)
{
    std::cout<< "copy constructor of Component.." << std::endl;
}

/** Is shared_pointer a good choice here?
 ** what will happen for the assignment?
 ** will parent be reclaimed automatically when the "real parent" is freed??
 */
void Component::setParent(const std::shared_ptr<Component>& _parent) {
    parent = _parent;
}

std::shared_ptr<Component> Component::getParent() const {
    return parent;
}
//void Component::alarm() {}
//
//void Component::activate() {}
//
//void Component::deactivate() {}

