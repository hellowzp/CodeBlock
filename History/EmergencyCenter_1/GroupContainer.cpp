#include "GroupContainer.h"
#include <iostream>
#include <algorithm>

GroupContainer::GroupContainer(const std::string _group, const std::string _id, bool _isActivated) :
    Component(_isActivated), group(_group), id(_id), parentContainer(nullptr), comps{}
{
    std::cout << "constructing GroupContainer.." << std::endl;
}

GroupContainer::~GroupContainer() {
    std::cout << "destructing GroupContainer.." << std::endl;
}

GroupContainer::GroupContainer(const GroupContainer& c):
    group(c.group), id(c.id), parentContainer(c.parentContainer), comps(c.comps)
{
    std::cout << "copy constructor in GroupContainer.." << std::endl;
}

void GroupContainer::addComponent(std::shared_ptr<Component> comp) {
	if (std::find(std::begin(comps), std::end(comps), comp) == std::end(comps))
        comps.push_back(comp);
}

void GroupContainer::removeComponent(std::shared_ptr<Component> comp) {
    comps.erase(std::remove(comps.begin(), comps.end(), comp), comps.end());
}

std::vector<std::shared_ptr<Component>> GroupContainer::getComponents() const {
	return comps;
}

void GroupContainer::alarm() const {
	for(auto comp : comps) {
        comp->alarm();
	}
}

void GroupContainer::activate() {
	for(auto comp : comps) {
        comp->activate();
	}
}

void GroupContainer::deactivate() {
	for(auto comp : comps) {
        comp->deactivate();
	}
}

void GroupContainer::describe() const {
    std::cout << "#####description#####\n" << "Group: " << group << ", id: " << "Parent:\n";
    if(parentContainer) {
        parentContainer->describe();
    } else {
        std::cout << "NULL" << std::endl;
    }
}

std::shared_ptr<GroupContainer> GroupContainer::getParent() const {
    return parentContainer;
}

void GroupContainer::setParent(std::shared_ptr<GroupContainer> pc) {
    parentContainer = pc;
}
