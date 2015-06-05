#include "GroupContainer.h"
#include <iostream>
#include <algorithm>

GroupContainer::GroupContainer(const std::string& _group, const std::string& _id):
    Component(_id), group(_group), comps{}
{
    std::cout << "constructing GroupContainer of group " << group << std::endl;
}

GroupContainer::~GroupContainer() {
    std::cout << "destructing GroupContainer ";
    this->describe();

    for(auto child : comps) {
        long int count = child.use_count();
        std::cout << "#child reference: " << count << "\n";
        child->setParent(nullptr);
        count = child.use_count();
        std::cout << "#child reference: " << count << "\n";
    }
}

GroupContainer::GroupContainer(const GroupContainer& c):
    Component(c.id,c.isActivated,c.parent), group(c.group), comps(c.comps)
{
    std::cout << "copy constructor of GroupContainer.." << std::endl;
}

//void GroupContainer::addComponent(const std::shared_ptr<Component>& comp) {
//	if (std::find(std::begin(comps), std::end(comps), comp) == std::end(comps)) {
//        comps.push_back(comp);
//        comp->setParent(std::make_shared<GroupContainer>(*this));
//        //pass-by-const-reference is equivalent to pass-by-value,
//        //impossible to change the original variable
//	}
//}

void GroupContainer::addComponent(std::shared_ptr<Component> comp) {
	long int count = comp.use_count();
    std::cout << "#comp reference: " << count << "\n";

	if (std::find(std::begin(comps), std::end(comps), comp) == std::end(comps)) {
        count = comp.use_count();
        std::cout << "#comp reference: " << count << "\n";

        comps.push_back(comp);
        count = comp.use_count();
        std::cout << "#comp reference: " << count << "\n";
        comp->setParent(std::make_shared<GroupContainer>(*this));
        count = comp.use_count();
        std::cout << "#comp reference: " << count << "\n";
        //comp->parent = std::make_shared<GroupContainer>(*this);
	}
}

void GroupContainer::removeComponent(std::shared_ptr<Component>& comp) {
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
    //std::cout << "#####description#####\n" << "Group: " << group << ", id: " id << "Parent:\n";
    std::cout << "@" << group << id << " ";
    if(parent) {
        parent->describe();
    } else {
        std::cout << std::endl;
    }
}
