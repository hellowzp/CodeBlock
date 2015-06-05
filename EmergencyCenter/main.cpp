#include <iostream>
#include <memory>
#include "MotionSensor.h"
#include "SmokeSensor.h"
#include "COSensor.h"
#include "GroupContainer.h"
#include "PoliceService.h"
#include "BuildingEvacuationService.h"
#include "FireService.h"
#include "CentralManager.h"

#include <stdlib.h>
#include <ctime>

using namespace std;

//bool __sort_by_id(const SensorComponent& s1, const SensorComponent& s2) {
//    int res =  s1.id.compare(s2.id);
//    return res==-1;
//}
//
//bool _sort_by_id(const shared_ptr<SensorComponent>& s1, const shared_ptr<SensorComponent>& s2) {
//    return __sort_by_id(*s1,*s2);
//}
//
//void sort_by_id(const vector<shared_ptr<SensorComponent>>& s) {
//    std::sort(s.begin(),s.end(),_sort_by_id);
//}

bool sort_by_id(const shared_ptr<SensorComponent>& s1, const shared_ptr<SensorComponent>& s2) {
    std::string id1 = s1->id;
    std::string id2 = s2->id;
    return id1.compare(id2) == -1;
}

int main(){

//    int* p = new int(5);
//    shared_ptr<int> sp1(p);
////    shared_ptr<int> sp1(*p); //error
//    shared_ptr<int> sp2(p);
//
//    long int count = sp1.use_count();
//    cout<<count<<"\n";
//
//    shared_ptr<int> sp3(sp1);
//    count = sp1.use_count();
//    cout<<count<<"\n";
//
//    auto level1 = make_shared<GroupContainer>("level", "#01");
//    auto level2 = make_shared<GroupContainer>("level", "#02");
//
//    { //just add scope for building
//    auto scs = make_shared<COSensor>("#001","vendorUnknown",8.5F);
//    auto sms = make_shared<MotionSensor>("#002","vendorUnknown",8.5F,12.5F);
//    auto fsv = make_shared<FireService>();
//    auto psv = make_shared<PoliceService>();
//
//    scs->addService(fsv);
//    scs->addService(psv);
//    sms->addService(fsv);
//
//    std::cout<< "...\nlevel adding sensors:\n";
//    level1->addComponent(scs);
//    level2->addComponent(sms);
//
//    std::cout<< "...\nbuilding adding levels:\n";
//    auto building =  make_shared<GroupContainer>("building", "#01");
//    building->addComponent(level1);
//    building->addComponent(level2);
//
//    cout<< "\nlevel1 description: " << level1->getFullName(true) << "..\n";
//    cout<< level1 << cout<< (*level1);
//    cout<< "level2 description: " << level2->getFullName() << "..\n";
//    cout<< "sensor1 description: " << scs->getFullName(true) << "..\n";
//    cout<< "sensor2 description: " << sms->getFullName(true) << "..\n\n";
//
//
//    long int count = building.use_count();
//    cout << "#building reference: " << count << "\n";
//    count = level1.use_count();
//    cout << "#level1 reference: " << count << "\n";
//    count = level2.use_count();
//    cout << "#level2 reference: " << count << "\n";
//    count = sms.use_count();
//    cout << "#sensor reference: " << count << "\n";
//    count = fsv.use_count();
//    sms->describe();
//    cout << "#service reference: " << count << "\n\n";
//
////    cout<< ms->getFullName() << ".\n";
////    cout<< ms->getFullName(true) << ".\n";
//
//    CentralManager mng{};
//    mng.addControllingCenter(building);
//    count = building.use_count();
//    cout << "#building reference: " << count << "\n";
//    count = level1.use_count();
//    cout << "#level1 reference: " << count << "\n";
//    count = level2.use_count();
//    cout << "#level2 reference: " << count << "\n\n";
//
//    //the search pattern can be improved by further using RE
////    mng.triggerComponentByFullName("building#01");
////    cout << "after triggering building..\n\n ";
////    mng.triggerComponentByFullName("building#01 level#01");
////    cout << "after triggering level..\n\n ";
//    mng.triggerComponentByFullName("building#01 level#01 Sensor#001");
//    cout << "after triggering sensor..\n\n";
//    }
//
//    count = level1.use_count();
//    cout << "\n#level1 reference: " << count << "\n";
//    count = level2.use_count();
//    cout << "#level2 reference: " << count << "\n\n";

//    delete (level.get());

    const int vendorNameLength = 8;
    const int aantal = 10000000;
    const int nrOfVendors = aantal / 1000;

    std::vector<std::shared_ptr<SensorComponent>> sensorcollection(aantal);
    //  sensorcollection.reserve(aantal);
    std::srand(static_cast<unsigned int>(time(0)));
    std::string vendors[nrOfVendors];
    for (int nr = 0; nr < nrOfVendors; ++nr)
    {
        for (int i = 0; i < vendorNameLength; ++i)
        {
            char next = (char)(97+std::rand()%26);
            vendors[nr].append(1, next);
        }
    }

    std::cout << "vendor init finished...\n";

    bool* used = new bool[aantal];
    for (int count = 0; count < aantal; ++count) used[count] = false;
    for (int count = 0; count < aantal; ++count)
    {
        int id = std::rand() % aantal;
        while (used[id] == true)
            id = std::rand() % aantal;
        used[id] = true;
        std::shared_ptr<SensorComponent> sensor;
        std::string sid = "#"+id;
        switch (std::rand() % 3)
        {
        case 0:
            sensor = std::make_shared<COSensor>(sid, vendors[std::rand()%4], 0.25f + (std::rand()%100)/1000.0f);
            break;
        case 1:
            sensor = std::make_shared<MotionSensor>(sid, vendors[std::rand()%4], 1.0f + std::rand()%15, 16.0f);
            break;
        case 2:
            sensor = std::make_shared<SmokeSensor>(sid, vendors[std::rand()%4], 0.000005f + (std::rand()%100)/100000.0f);
            break;
        }
        sensorcollection[count] = sensor;
//        if (id % 2) ++sensor;
    }

    std::cout << "sensor init finished...\n";

    for (int i =0; i<30; i++)
    {
        std::cout << "Sensor of group \n" << static_cast<std::shared_ptr<SensorComponent>>(sensorcollection.at(i));
    }

    std::cout << "sorting sensors...\n";
    sort(sensorcollection.begin(), sensorcollection.end(),sort_by_id );
    std::cout << "\n\n\n\n";

    for (int i =0; i<30; i++)
    {
        std::cout << "Sensor of group \n" << static_cast<std::shared_ptr<SensorComponent>>(sensorcollection.at(i));
    }

    return 0;
}



