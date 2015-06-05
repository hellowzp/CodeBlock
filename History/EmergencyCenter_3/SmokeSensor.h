#ifndef SMOKESENSOR_H
#define SMOKESENSOR_H

#include "SensorComponent.h"


class SmokeSensor : public SensorComponent
{
    private:
        float sensitivity;

    public:

        SmokeSensor(const std::string& _id, const std::string& vn, const float sensibility);

        ~SmokeSensor();

        SmokeSensor(const SmokeSensor& other);

        SmokeSensor& operator=(const SmokeSensor& other);

        void describe() const;

};

#endif // SMOKESENSOR_H
