#ifndef COSENSOR_H
#define COSENSOR_H

#include "SensorComponent.h"

#include <memory>
#include <vector>
#include <string>

class COSensor : public SensorComponent {

private:
    float concentration;

public:
    COSensor(const std::string& _id, const std::string& vn, float _value);

	~COSensor();

	COSensor(const COSensor& cos);

	void describe() const;
};

#endif // COSENSOR_H

