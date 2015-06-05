#include "GroupContainer.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <stack>
#include <cstring>

GroupContainer::GroupContainer(const std::string& _group, const std::string& _id):
    Component(_id), group(_group)
{
    std::cout << "constructing GroupContainer of " << group << id << std::endl;
//    std::cout << comps.size() << std::endl;
}

GroupContainer::~GroupContainer() {
    std::cout << "destructing GroupContainer " + this->getFullName() + "\n";

    for(auto child : comps) {
        long int count = child.use_count();
        std::cout << "#child reference: " << count << "\n";
        child->setParent(nullptr);
//        count = child.use_count();
//        std::cout << "#child reference: " << count << "\n";
    }
}

GroupContainer::GroupContainer(const GroupContainer& other):
    Component(other.id), group(other.group), comps(other.comps)
{
    std::cout << "copy constructor of GroupContainer from " << group << id <<std::endl;
    isActivated = other.isActivated;
    parent = other.parent;
}

void GroupContainer::addComponent(std::shared_ptr<Component> comp) {
	long int count = comp.use_count();
    std::cout << "#comp reference: " << count << "\n";

//    if(c) {
//        comps.pop_back();
//    }

	if (std::find(std::begin(comps), std::end(comps), comp) == std::end(comps)) {
        count = comp.use_count();
        std::cout << "#comp reference: " << count << "\n";

        comps.push_back(comp);
        count = comp.use_count();
        std::cout << "#comp reference: " << count << "\n";
//        comp->setParent(std::make_shared<GroupContainer>(this->group,this->id));
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
        if(comp!=std::shared_ptr<Component>(nullptr))
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

const std::string GroupContainer::getFullName(bool reverse) const {
//    std::ostringstream os;
//    std::string res;
//    os << "@" << group << id << " ";
//    if(parent) {
//        res = os.str() + parent->getFullName();
//    }
//    return res;
    std::string res = group + id + " ";
    if(parent) res += parent->getFullName();
    if(!reverse) return res;
    else {
        std::string result;
//        int size = res.length();
//        int i = 0;
//        for(int j=0; j<size; j++) {
//            if(res[j]== 32) {
//                for(int k=i; k<j; k++) {
//                    result[size-j+k-i] = res[k];
//                }
//                result[size-j-1] = 32;
//                i = j;
//            }
//        }
//
//        for(int k=i; k<size; k++) {
//            result[size-i+k-i] = res[k];
//        }

        std::string buf; // Have a buffer string
        std::stringstream ss(res); // Insert the string into a stream
        std::stack<std::string> tokens; // Create vector to hold our words

        while (ss >> buf) tokens.push(buf);
        while(tokens.size()>=1) {
            result += tokens.top() + " ";
            tokens.pop();
        }
        return result;
    }
}

std::shared_ptr<Component> GroupContainer::searchComponentByFullName(const std::string& fname){
    std::string fullName = this->getFullName(true);
    fullName.resize(fullName.length()-1); //remove the last space
    std::cout<< "searching for "  << fname << " in " << fullName << "..\n";
    if(fullName.compare(fname)==0) {
        return std::make_shared<GroupContainer>(*this);
    }
    char* cstr1 = new char[fname.length()+1];
    char* cstr2 = new char[fullName.length()+1];
    std::strcpy(cstr1, fname.c_str());
    std::strcpy(cstr2, fullName.c_str());
    if(std::strcmp(cstr1,cstr2)==0) {
        return std::make_shared<GroupContainer>(*this);
    }
    delete cstr1;
    delete cstr2;

    std::size_t pos = fname.find(fullName);
    if( pos!=std::string::npos){
        for(auto child : comps) {
            auto found = child->searchComponentByFullName(fname);
            if(found!=std::shared_ptr<Component>{nullptr}) return found;
        }
    }
    return {nullptr};
}
