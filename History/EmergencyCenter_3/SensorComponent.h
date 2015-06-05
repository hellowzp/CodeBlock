#ifndef SENSORCOMPONENT_H
#define SENSORCOMPONENT_H

#include "Component.h"
#include "EmergencyService.h"
#include <string>
#include <memory>
#include <vector>

class SensorComponent : public Component {

    typedef std::vector<std::shared_ptr<EmergencyService>> vSpSVC;

protected:
	const std::string vendorName;
	vSpSVC services;

public:

    SensorComponent(const std::string& _id, const std::string& vn);

    SensorComponent(const std::string& _id, const std::string& vn, const vSpSVC& svc);

    virtual ~SensorComponent() = 0;

    SensorComponent(const SensorComponent& sc);

	void addService(const std::shared_ptr<EmergencyService>& svc);

	void removeService(std::shared_ptr<EmergencyService>& svc);

    void removeAllServices();

    std::vector<std::shared_ptr<EmergencyService>> getServices() const;

	void alarm() const;

	void activate();

	void deactivate();

	void alarmService(std::shared_ptr<EmergencyService>& src);

	virtual void describe() const;

	const std::string getFullName(bool reverse = false) const;

	std::shared_ptr<Component> searchComponentByFullName(const std::string& fname);
};

#endif // SENSORCOMPONENT_H

