#ifndef EVACUATIONSERVICE_H
#define EVACUATIONSERVICE_H

#include <EmergencyService.h>

class EvacuationService : public EmergencyService
{
    public:
        EvacuationService();
        ~EvacuationService();
        EvacuationService(const EvacuationService& other);
        EvacuationService& operator=(const EvacuationService& other);

        void performService();

    protected:
    private:
};

#endif // EVACUATIONSERVICE_H
