#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>

class Component {

protected:
    bool isActivated;
    std::shared_ptr<Component> parent;

//    void setParent(std::shared_ptr<Component> _parent);

public:
    Component(bool _isActivated = true);

    virtual ~Component();

    virtual void alarm() const = 0;

    virtual void activate() = 0;

    virtual void deactivate() = 0;

    virtual void describe() const = 0;

//     virtual void alarm() ;
//
//     virtual void activate() ;
//
//     virtual void deactivate() ;

};


#endif
