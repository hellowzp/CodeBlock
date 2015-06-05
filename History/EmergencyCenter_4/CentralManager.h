#ifndef CENTRALMANAGER_H
#define CENTRALMANAGER_H

#include "Component.h"
#include <vector>
#include <memory>

class CentralManager
{
    typedef std::vector<std::shared_ptr<Component>> VSptr;
    public:
        CentralManager();
        ~CentralManager();
        CentralManager(const CentralManager& other);
        CentralManager& operator=(const CentralManager& other);

        void triggerComponentByFullName(const std::string& fname) const;
        void triggerAll() const;
        void addControllingCenter(std::shared_ptr<Component> cc);

    protected:
    private:
        VSptr manager;
};

#endif // CENTRALMANAGER_H
