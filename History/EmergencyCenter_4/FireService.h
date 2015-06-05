#ifndef FIRESERVICE_H
#define FIRESERVICE_H

#include <EmergencyService.h>


class FireService : public EmergencyService
{
    public:
        FireService();
        virtual ~FireService();

        void performService();

    protected:
    private:
};

#endif // FIRESERVICE_H
