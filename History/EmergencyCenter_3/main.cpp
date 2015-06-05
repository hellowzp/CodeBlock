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

    auto level = make_shared<GroupContainer>("level", "#01");

    { //just add scope for building
    auto building =  make_shared<GroupContainer>("building", "#01");
    building->addComponent(level);
    cout<< "\nlevel description: " << level->getFullName(true) << "\n";

    auto scs = make_shared<COSensor>("#001","vendorUnknown",8.5F);
    std::cout<< "..." << endl;

    level->addComponent(scs);
    std::cout<< "..." << endl;

    auto svc = make_shared<FireService>();
    scs->addService(svc);
    scs->addService(make_shared<PoliceService>());
    std::cout<< "..." << endl;

    long int count = building.use_count();
    cout << "#building reference: " << count << "\n";
    count = level.use_count();
    cout << "#level reference: " << count << "\n";
    count = svc.use_count();
    cout << "#FireService reference: " << count << "\n";


    auto level2 = make_shared<GroupContainer>("level", "#02");
    auto ms = make_shared<MotionSensor>("#002","vendorUnknown",8.5F,12.5F);
    ms->addService(svc);
    level2->addComponent(ms);
    building->addComponent(level2);
    level2->describe();

    cout<< ms->getFullName() << ".\n";
    cout<< ms->getFullName(true) << ".\n";

    CentralManager mng{};
    mng.addControllingCenter(building);
    count = building.use_count();
    cout << "#building reference: " << count << "\n";
    count = level.use_count();
    cout << "#level reference: " << count << "\n";

    //the search pattern can be improved by further using RE
    mng.triggerComponentByFullName("building#01");
    mng.triggerComponentByFullName("building#01 level#01");
    mng.triggerComponentByFullName("building#01 level#01 Sensor#001");
    }

    long int count = level.use_count();
    cout << "#level reference: " << count << "\n";

//    delete (level.get());

    return 0;
}
