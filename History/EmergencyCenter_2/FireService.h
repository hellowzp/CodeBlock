#ifndef FIRESENSOR_H
#define FIRESENSOR

#include "EmergencyService.h"

class FireService : public EmergencyService {

public:
    ~FireService();

    void performService();
};

#endif
