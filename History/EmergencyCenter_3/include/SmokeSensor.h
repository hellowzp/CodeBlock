#ifndef SMOKESENSOR_H
#define SMOKESENSOR_H

#include "SensorComponent.h"


class SmokeSensor : public SensorComponent
{
    public:
        /** Default constructor */
        SmokeSensor();
        /** Default destructor */
        ~SmokeSensor();
        /** Copy constructor
         *  \param other Object to copy from
         */
        SmokeSensor(const SmokeSensor& other);
        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        SmokeSensor& operator=(const SmokeSensor& other);
    protected:
    private:
};

#endif // SMOKESENSOR_H
