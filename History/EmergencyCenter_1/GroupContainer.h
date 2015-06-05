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

    const std::string group;
    const std::string id;
    std::shared_ptr<GroupContainer> parentContainer;
    std::vector<std::shared_ptr<Component>> comps;

public:
    GroupContainer(const std::string group, const std::string _id, bool _isActivated = true );

	~GroupContainer();

	GroupContainer(const GroupContainer& c);

    void addComponent(std::shared_ptr<Component> com);

	void removeComponent(std::shared_ptr<Component> com);

	std::vector<std::shared_ptr<Component>> getComponents() const;

	void alarm() const;

	void activate();

	void deactivate();

	void describe() const;

	std::shared_ptr<GroupContainer> getParent() const;

	void setParent(std::shared_ptr<GroupContainer> pc);
};

#endif // GROUPCONTAINER_H
