#ifndef EMERGENCYSERVICE_H
#define EMERGENCYSERVICE_H

class EmergencyService {

public:
    virtual ~EmergencyService();
	virtual void performService() = 0;
};

#endif
