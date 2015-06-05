#include "CentralManager.h"
#include <algorithm>
#include <iostream>

CentralManager::CentralManager()
{
//    std::cout << manager.size();
}

CentralManager::~CentralManager()
{
    //dtor
}

CentralManager::CentralManager(const CentralManager& other):
    manager{other.manager}
{
    //copy ctor
}

CentralManager& CentralManager::operator=(const CentralManager& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void CentralManager::triggerComponentByFullName(const std::string& fname) const {
//    std::cout << manager.size();
    for(auto child : manager) {
        auto found = child->searchComponentByFullName(fname);
        if(found) {
            found->alarm();
            return;
        }
    }
    std::cout << "Component with full name " << fname << " is not found...\n";
}

void CentralManager::addControllingCenter(std::shared_ptr<Component> man) {
    if (std::find(std::begin(manager), std::end(manager), man) == std::end(manager)) {
        manager.push_back(man);
	}
}

