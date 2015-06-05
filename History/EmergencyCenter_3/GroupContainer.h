#ifndef GROUPCONTAINER_H
#define GROUPCONTAINER_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "Component.h"

enum GROUP { BUILDING, LEVEL, ROOM };

class GroupContainer : public Component {

private:
//    using Component::parent;
    const std::string group;
    std::vector<std::shared_ptr<Component>> comps;

public:
    GroupContainer(const std::string& group, const std::string& _id);

	~GroupContainer();

	GroupContainer(const GroupContainer& c);

    void addComponent(std::shared_ptr<Component> com);

	void removeComponent(std::shared_ptr<Component>& com);

	std::vector<std::shared_ptr<Component>> getComponents() const;

	void alarm() const;

	void activate();

	void deactivate();

	void describe() const;

	const std::string getFullName(bool reverse = false) const;

	std::shared_ptr<Component> searchComponentByFullName(const std::string& fname);

};

#endif // GROUPCONTAINER_H
