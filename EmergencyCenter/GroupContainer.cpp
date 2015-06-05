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

    comp->describe(); //test whether the copy constructor works as expected

	if (std::find(std::begin(comps), std::end(comps), comp) == std::end(comps)) {
        count = comp.use_count();
        std::cout << "#comp reference: " << count << "\n";

        comps.push_back(comp);
        count = comp.use_count();
        std::cout << "#comp reference: " << count << "\n";

        //this will call the copy constructor of GroupContainer and
        //make a copy of the current object pointed to by this,
        //also comp's parent will hold the reference to the copy
        //instead of the current object which is obviously not wanted
        //even though it avoids the problem of cycle reference
//        auto this_sp = std::make_shared<GroupContainer>(*this);
//        count = comp.use_count();
//        std::cout << "#comp reference: " << count << "\n";
//        count = this_sp.use_count();
//        std::cout << "#parent reference: " << count << "\n";
//        comp->setParent(this_sp);
//        count = this_sp.use_count();
//        std::cout << "#parent reference: " << count << "\n";

        comp->setParent(shared_from_this());

        count = comp.use_count();
        std::cout << "#comp reference: " << count << "\n";
        //comp->parent = std::make_shared<GroupContainer>(*this);

        comp->describe(); //test whether the copy constructor works as expected
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
    std::cout << "@" << group << id;
    if(parent) {
        std::cout << " ";
        parent->describe();
    } else {
        std::cout << "\n";
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
    std::string res = group + id;
    if(parent) res += " " + parent->getFullName();
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
        result.resize(result.length()-1);
        std::cout<< "full name: " << result << "..\n"; //remove the last space
        return result;
    }
}

std::shared_ptr<Component> GroupContainer::searchComponentByFullName(const std::string& fname){
    std::string fullName = this->getFullName(true);
    std::cout<< "searching for "  << fname << " in " << fullName << "..\n";
    if(fullName.compare(fname)==0) {
        return std::make_shared<GroupContainer>(*this);
    }
//    char* cstr1 = new char[fname.length()+1];
//    char* cstr2 = new char[fullName.length()+1];
//    std::strcpy(cstr1, fname.c_str());
//    std::strcpy(cstr2, fullName.c_str());
//    if(std::strcmp(cstr1,cstr2)==0) {
//        return std::make_shared<GroupContainer>(*this);
//    }
//    delete cstr1;
//    delete cstr2;

    std::size_t pos = fname.find(fullName);
    if( pos!=std::string::npos){
        for(auto child : comps) {
            auto found = child->searchComponentByFullName(fname);
            if(found!=std::shared_ptr<Component>{nullptr}) return found;
        }
    }
    return {nullptr};
}

std::ostream& operator<<(std::ostream& os, GroupContainer& com){
    os << "@" << com.group << com.id;
    auto parent = com.getParent();
    if(parent != nullptr ) {
        os << " " << *(dynamic_cast<GroupContainer*>(parent.get()));
    } else {
        os << "\n";
    }
    return os;
}
