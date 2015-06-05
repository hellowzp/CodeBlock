#include <iostream>
#include <memory>
#include "COSensor.h"
#include "GroupContainer.h"
#include "PoliceService.h"
#include "BuildingEvacuationService.h"
#include "FireService.h"

using namespace std;

int main(){

    auto level = make_shared<GroupContainer>("level", "#01");

    { //just add scope for building
    auto building =  make_shared<GroupContainer>("building", "#01");
    building->addComponent(level);

//    COSensor cs{"#001",8.5F};
//    std::cout<< "..." << endl;

//    auto scs = make_shared<COSensor>("#001",8.5F);
//    std::cout<< "..." << endl;
//
//    level->addComponent(scs);
//    std::cout<< "..." << endl;
//
//    auto svc = make_shared<FireService>();
//    scs->addService(svc);
//
//    scs->addService(make_shared<PoliceService>());

//    SensorComponent ss{"#002"};
//    level->addComponent(make_shared<SensorComponent>(ss));
//    ss->addService(svc);

//    level->alarm();
//    building->alarm();

//    long int count = building.use_count();
//    cout << "#building reference: " << count << "\n";
//
//    count = level.use_count();
//    cout << "#level reference: " << count << "\n";
    }
//
//    long int count = level.use_count();
//    cout << "#level reference: " << count << "\n";

//    level->setParent(nullptr);
    long int count = level.use_count();
    cout << "#level reference: " << count << "\n";

//    delete (level.get());

    return 0;
}
