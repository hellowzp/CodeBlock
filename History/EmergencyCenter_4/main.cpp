#include <iostream>
#include <memory>
#include "MotionSensor.h"
#include "COSensor.h"
#include "GroupContainer.h"
#include "PoliceService.h"
#include "BuildingEvacuationService.h"
#include "FireService.h"
#include "CentralManager.h"

using namespace std;

int main(){

    auto level1 = make_shared<GroupContainer>("level", "#01");
    auto level2 = make_shared<GroupContainer>("level", "#02");

    { //just add scope for building
    auto scs = make_shared<COSensor>("#001","vendorUnknown",8.5F);
    auto sms = make_shared<MotionSensor>("#002","vendorUnknown",8.5F,12.5F);
    auto fsv = make_shared<FireService>();
    auto psv = make_shared<PoliceService>();
    scs->addService(fsv);
    scs->addService(psv);
    sms->addService(fsv);
    std::cout<< "...\nlevel adding sensors:\n";

    level1->addComponent(scs);
    level2->addComponent(sms);

    std::cout<< "...\nbuilding adding levels:\n";
    auto building =  make_shared<GroupContainer>("building", "#01");
    building->addComponent(level1);
    building->addComponent(level2);

    cout<< "\nlevel1 description: " << level1->getFullName(true) << "\n";
    cout<< "level2 description: " << level2->getFullName(true) << "\n";
    cout<< "sensor1 description: " << scs->getFullName(true) << "\n";
    cout<< "sensor2 description: " << sms->getFullName(true) << "\n\n";


    long int count = building.use_count();
    cout << "#building reference: " << count << "\n";
    count = level1.use_count();
    cout << "#level1 reference: " << count << "\n";
    count = level2.use_count();
    cout << "#level2 reference: " << count << "\n";
    count = sms.use_count();
    cout << "#sensor reference: " << count << "\n";
    count = fsv.use_count();
    sms->describe();
    cout << "#service reference: " << count << "\n\n";

//    cout<< ms->getFullName() << ".\n";
//    cout<< ms->getFullName(true) << ".\n";

    CentralManager mng{};
    mng.addControllingCenter(building);
    count = building.use_count();
    cout << "#building reference: " << count << "\n";
    count = level1.use_count();
    cout << "#level1 reference: " << count << "\n";
    count = level2.use_count();
    cout << "#level2 reference: " << count << "\n\n";

    //the search pattern can be improved by further using RE
//    mng.triggerComponentByFullName("building#01");
//    cout << "after triggering building..\n\n ";
//    mng.triggerComponentByFullName("building#01 level#01");
//    cout << "after triggering level..\n\n ";
    mng.triggerComponentByFullName("building#01 level#01 Sensor#001");
    cout << "after triggering sensor..\n\n ";
    }

    long int count = level1.use_count();
    cout << "\n#level1 reference: " << count << "\n";
    count = level2.use_count();
    cout << "#level2 reference: " << count << "\n\n";

//    delete (level.get());

    return 0;
}
