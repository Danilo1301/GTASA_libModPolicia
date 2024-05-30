#include "Locations.h"

#include "Mod.h"
#include "CleoFunctions.h"
#include "Log.h"

std::vector<CVector> Locations::m_Houses = {
    //LS
    {2132.2500,-1280.7600,25.8906},
    {2017.8800,-1629.9399,13.7121},
    {893.6480,-1635.9600,14.9297},

    //LV
    {1596.4000,2093.6599,11.3125},
    {2177.5071,736.0220,11.4609},
    {986.1868,2000.5649,11.4609},

    //SF
    {-2881.3601,790.4580,35.1238},
    {-2792.1499,-35.7933,7.8594},
    {-2065.9700,1159.8300,46.6484}
};

std::vector<CVector> Locations::m_Stores = {
    //LS
};

CVector Locations::GetRandomHouse()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "GetRandomHouse" << std::endl;

    std::vector<CVector> locations;
    for(auto house : m_Houses)
    {
        locations.push_back(house);
    }

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    std::sort(locations.begin(), locations.end(), [playerActor](CVector a, CVector b) {
        return Mod::DistanceFromPed(playerActor, b) > Mod::DistanceFromPed(playerActor, a);
    });

    for(auto location : locations)
    {
        auto distance = Mod::DistanceFromPed(playerActor, location);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Location distance: " << distance << std::endl;
    }

    int maxIndex = locations.size() - 1;
    if(maxIndex > 3) maxIndex = 3;

    auto randomLocation = locations[Mod::GetRandomNumber(0, maxIndex)];

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Location selected: " << randomLocation.x << ", " << randomLocation.y << ", " << randomLocation.z << std::endl;

    return randomLocation;
}