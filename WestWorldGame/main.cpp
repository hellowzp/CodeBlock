#include "Locations.h"
#include "Miner.h"
#include "misc/ConsoleUtils.h"
#include "EntityNames.h"


int main()
{
  //create a miner
  Miner miner(ent_Miner_Bob);

  //simply run the miner through a few Update calls
  for (int i=0; i<5; ++i)
  {
    miner.Update();

    Sleep(800);
  }

  //wait for a key-press before exiting
  PressAnyKeyToContinue();

  return 0;
}
