//Insert following code in your main.cpp.
//It may be possible that you need to adapt the code a little bit due to:
//  - different names for sensorclasses
//  - different constructors for sensorclasses
//Fill in the missing functor or lambda
//
//There is one big mistake in this code, letting your program crash.
//What is the reason. Find a solution.
//
//You can also run the analyze tools from within QtCreator
//-Valgrind Memory Analysis: should give no problems due to use of smart pointers
//-Valgrind Funtion Profiler: gives you an idea in which parts of your code most time is spent
//


#include <stdlib.h>
  //Performance test for sorting
  const int vendorNameLength = 8;
  const int aantal = 10000000;
  const int nrOfVendors = aantal / 1000;


  std::vector<std::shared_ptr<SensorComponent>> sensorcollection(aantal);
  //  sensorcollection.reserve(aantal);
  std::srand(static_cast<uint>(time(0)));
  std::string vendors[nrOfVendors];
  for (int nr = 0; nr < nrOfVendors; ++nr)
    {
    for (int i = 0; i < vendorNameLength; ++i)
      {
      char next = (char)(97+std::rand()%26);
      vendors[nr].append(1, next);
      }
    }
  bool used[aantal];
  for (int count = 0; count < aantal; ++count) used[count] = false;
  for (int count = 0; count < aantal; ++count)
    {
    int id = std::rand() % aantal;
    while (used[id] == true)
      id = std::rand() % aantal;
    used[id] = true;
    std::shared_ptr<SensorComponent> sensor;
    switch (std::rand() % 3)
      {
      case 0: sensor = std::make_shared<COSensor>("#"+id, vendors[std::rand()%4], 0.25f + (std::rand()%100)/1000.0f);
        break;
      case 1: sensor = std::make_shared<MotionSensor>("#"+id, vendors[std::rand()%4], 1 + std::rand()%15);
        break;
      case 2: sensor = std::make_shared<SmokeSensor>("#"+id, vendors[std::rand()%4], 0.000005 + (std::rand()%100)/100000.0f);
        break;
      }
    sensorcollection[count] = sensor;
    if (id % 2)
      ++sensor;
    }
  for (int i =0; i<50; i++){

    std::cout << "Sensor of group \n" << static_cast<std::shared_ptr<SensorComponent>>(sensorcollection.at(i));
    }
  sort(sensorcollection.begin(), sensorcollection.end(), [provide here functor or lambda to sort on id, vendor and on both...]);
  std::cout << "\n\n\n\n";
  for (int i =0; i<50; i++){

    std::cout << "Sensor of group \n" << static_cast<std::shared_ptr<SensorComponent>>(sensorcollection.at(i));
    }


