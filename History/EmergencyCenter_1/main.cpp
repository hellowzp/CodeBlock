#include <iostream>
#include <memory>
#include "COSensor.h"
#include "GroupContainer.h"
#include "PoliceService.h"
#include "BuildingEvacuationService.h"
#include "FireService.h"

using namespace std;

int main(){
    cout << "main" << endl;

//    shared_ptr<Component> building = make_shared<GroupContainer>("building");
//    shared_ptr<Component> level = make_shared<GroupContainer>("level");
//    ((shared_ptr<GroupContainer>)building)->addComponent(level);

//    shared_ptr<GroupContainer> building = make_shared<GroupContainer>("building");
//    shared_ptr<GroupContainer> level = make_shared<GroupContainer>("level");
//    building->addComponent(level);
//    level->addComponent(make_shared<COSensor>(new FireService()));
//    building->alarm();

//    GroupContainer* building = new GroupContainer("building", "#01");
//    GroupContainer* level = new GroupContainer("level", "#01");
//
//    building->describe();
//    level->describe();
//
//    level->setParent(building);
//    level->describe();
//
//    delete building;
//    delete level;

    auto building =  make_shared<GroupContainer>("building", "#01");
    auto level = make_shared<GroupContainer>("level", "#01");

    building->describe();
    level->describe();

    level->setParent(building);
    level->describe();

    return 0;
}
