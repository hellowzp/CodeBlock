#ifndef MOTIONSENSOR_H_INCLUDED
#define MOTIONSENSOR_H_INCLUDED

#include "SensorComponent.h"

#include <memory>
#include <vector>
#include <string>

class MotionSensor : public SensorComponent {

private:
    float min;
    float max;

public:
    MotionSensor(const std::string& _id, const std::string& vn,
                 const float _min, const float _max);

	~MotionSensor();

	MotionSensor(const MotionSensor& cos);

	MotionSensor& operator=(const MotionSensor& other);

//	void alarm();
//
//	void activate();
//
//	void deactivate();

	void describe() const;
};

#endif // MOTIONSENSOR_H_INCLUDED
