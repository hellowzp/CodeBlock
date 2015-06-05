#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>
#include <string>

class Component {

public:
    const std::string id;  //STH LIKE #001
    bool isActivated;
    std::shared_ptr<Component> parent;

public:
    Component(const std::string& _id);

//    Component(const std::string& _id, bool _isAct, const std::shared_ptr<Component>& _parent);

    virtual ~Component();

    Component(const Component& comp);

    virtual void alarm() const = 0;

    virtual void activate() = 0;

    virtual void deactivate() = 0;

    virtual void describe() const = 0;

    std::shared_ptr<Component> getParent() const ;

    /** only for internal use when the parent call addComponent()
     *  but why it cannot be protected??
     */
    void setParent(const std::shared_ptr<Component>& _parent);

    virtual const std::string getFullName(bool reverse = false) const = 0;

    virtual std::shared_ptr<Component> searchComponentByFullName(const std::string& fname) = 0;

};


#endif
