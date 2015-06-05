#ifndef POLICESERVICE_H
#define POLICESERVICE_H

#include "EmergencyService.h"

class PoliceService : public EmergencyService {

public:
    ~PoliceService();

    void performService();
};

#endif
