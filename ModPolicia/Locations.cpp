#include "Locations.h"

#include "Mod.h"
#include "CleoFunctions.h"
#include "Log.h"

std::vector<Location> Locations::m_Houses = {
    //LS
    {{2132.2500,-1280.7600,25.8906}, 0, CITY::LOS_SANTOS},
    {{2017.8800,-1629.9399,13.7121}, 0, CITY::LOS_SANTOS},
    {{893.6480,-1635.9600,14.9297}, 0, CITY::LOS_SANTOS},

    //LV
    {{1596.4000,2093.6599,11.3125}, 0, CITY::LAS_VENTURAS},
    {{2177.5071,736.0220,11.4609}, 0, CITY::LAS_VENTURAS},
    {{986.1868,2000.5649,11.4609}, 0, CITY::LAS_VENTURAS},

    //SF
    {{-2881.3601,790.4580,35.1238}, 0, CITY::SAN_FIERRO},
    {{-2792.1499,-35.7933,7.8594}, 0, CITY::SAN_FIERRO},
    {{-2065.9700,1159.8300,46.6484}, 0, CITY::SAN_FIERRO}
};

std::vector<Location> Locations::m_Stores = {
    //LS
};

std::vector<Location> Locations::m_PoliceDepartments = {
    {{1536.1325, -1671.2093, 13.3828}, 0, CITY::LOS_SANTOS}, //dp ls
    {{635.1006, -571.8163, 16.3359}, 0, CITY::LOS_SANTOS}, //dp dillmore
    {{-1606.5406, 721.6789, 12.1554}, 0, CITY::SAN_FIERRO}, //dp sf
    {{-211.8363, 978.0092, 19.3001}, 0, CITY::LAS_VENTURAS}, //dp fort carson
    {{2289.8784, 2425.8894, 10.8203}, 0, CITY::LAS_VENTURAS} //dp lv
};

CVector Locations::GetRandomHouse()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "GetRandomHouse" << std::endl;

    auto city = GetClosestCity();

    std::vector<Location> locations;
    for(auto house : m_Houses)
    {
        if(house.city != city) continue;

        locations.push_back(house);
    }

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    std::sort(locations.begin(), locations.end(), [playerActor](Location a, Location b) {
        return DistanceFromPed(playerActor, b.position) > DistanceFromPed(playerActor, a.position);
    });

    for(auto location : locations)
    {
        auto distance = DistanceFromPed(playerActor, location.position);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Location distance: " << distance << std::endl;
    }

    //int maxIndex = locations.size() - 1;
    //if(maxIndex > 3) maxIndex = 3;

    auto randomLocation = locations[GetRandomNumber(0, locations.size() - 1)];

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Location selected: " << CVectorToString(randomLocation.position) << std::endl;

    return randomLocation.position;
}

CITY Locations::GetClosestCity()
{
    CITY closestCity = CITY::LOS_SANTOS;
    double closestDistance = INFINITY;

    for(size_t i = 0; i < m_PoliceDepartments.size(); i++)
    {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &playerX, &playerY, &playerZ);

        auto dpLocation = m_PoliceDepartments[i];

        auto distance = DistanceBetweenPoints(dpLocation.position, CVector(playerX, playerY, playerZ));

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closestCity = dpLocation.city;
        }
    }

    return closestCity;
}

Location Locations::GetClosestPoliceDepartment()
{
    Location closest = m_PoliceDepartments[0];
    double closestDistance = INFINITY;

    for(size_t i = 0; i < m_PoliceDepartments.size(); i++)
    {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &playerX, &playerY, &playerZ);

        auto dpLocation = m_PoliceDepartments[i];

        auto distance = DistanceBetweenPoints(dpLocation.position, CVector(playerX, playerY, playerZ));

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closest = dpLocation;
        }
    }

    return closest;
}