#include "BikePickpocket.h"

#include "CleoFunctions.h"
#include "Skins.h"
#include "Ped.h"
#include "Peds.h"
#include "Vehicle.h"
#include "Vehicles.h"
#include "Chase.h"
#include "Pullover.h"
#include "Mod.h"
#include "Callouts.h"
#include "Log.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

float BikePickpocket::m_TimeBetweenPickpockets = 180 * 1000;

CVector pickpocketPosition = CVector(0, 0, 0);
int timeToSpawnPickpocket = -1;
int timeToAlert = 0;
bool pickpocketInProgress = false;

Ped* victim = NULL;
Ped* driver = NULL;
Ped* gunner = NULL;
Vehicle* bike = NULL;

bool enteringBike = false;

void BikePickpocket::Update(int dt)
{
    //menuVSL->debug->visible = true;

    UpdateSpawnPickpocket(dt);

    if(pickpocketInProgress)
    {
        auto playerPosition = GetPedPosition(GetPlayerActor());

        auto distance = DistanceBetweenPoints(playerPosition, pickpocketPosition);

        if(distance < 15 && !enteringBike)
        {
            enteringBike = true;
            
            //05CA: AS_actor $GUNNER enter_car $BIKE passenger_seat 0 time 10000

            CleoFunctions::ACTOR_ENTER_CAR_PASSENGER_SEAT(gunner->hPed, bike->hVehicle, 10000, 0);
        }

        if(enteringBike)
        {
            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(gunner->hPed))
            {
                enteringBike = false;

                FinishPickpocket();

                Chase::MakeCarStartRunning(bike, driver);
            }
        }
    }

    if(pickpocketInProgress)
    {
        if(!CleoFunctions::ACTOR_DEFINED(driver->hPed))
        {
            FinishPickpocket();
        }
    }

    if(!pickpocketInProgress && victim)
    {
        if(CleoFunctions::ACTOR_DEFINED(victim->hPed))
        {
            victim->shouldHandsup = false;
            ClearPedAnimations(victim->hPed);
            victim = NULL;
        } else {
            victim = NULL;
        }
    }
}

void BikePickpocket::UpdateSpawnPickpocket(int dt)
{
    if(pickpocketInProgress) return;
    if(Chase::m_ChasingPed) return;
    if(Callouts::IsOnCallout()) return;
    if(Pullover::m_PullingPed || Pullover::m_PullingVehicle) return;
    if(!Mod::m_Enabled) return;
        
    if(timeToSpawnPickpocket == -1) timeToSpawnPickpocket = BikePickpocket::m_TimeBetweenPickpockets;

    timeToSpawnPickpocket -= dt;

    timeToAlert -= dt;
    if(timeToAlert <= 0)
    {
        timeToAlert = 5000;
        menuVSL->debug->AddLine("spawning pickpocket in " + std::to_string(timeToSpawnPickpocket));
    }

    if(timeToSpawnPickpocket <= 0)
    {
        timeToSpawnPickpocket = 0;

        SpawnPickpocketCloseToPlayer();
    }
}

void BikePickpocket::SpawnPickpocketCloseToPlayer()
{
    menuVSL->debug->AddLine("spawning pickpocket");

    auto position = GetPedPositionWithOffset(GetPlayerActor(), CVector(0, 80, 0));

    float nodeX, nodeY, nodeZ;
    CleoFunctions::STORE_PED_PATH_COORDS_CLOSEST_TO(position.x, position.y, position.z, &nodeX, &nodeY, &nodeZ);
    CVector nodePosition = CVector(nodeX, nodeY, nodeZ);

    SpawnPickpocket(nodePosition);
}

void BikePickpocket::SpawnPickpocket(CVector position)
{
    enteringBike = false;
    pickpocketPosition = position;
    pickpocketInProgress = true;

    //test: TestPickpocket


    //009A: $VICTIM = create_actor_pedtype 23 model 12 at 1@ 2@ 3@
    auto victimSkin = Skins::GetRandomSkin(SkinGenre::BOTH, SkinGang::GANG_NONE);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "Spawning pickpocket, skin: " << victimSkin.modelId << std::endl;

    auto victimHandle = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, victimSkin.modelId, position.x, position.y, position.z);
    victim = Peds::TryCreatePed(victimHandle);
    victim->shouldHandsup = true;

    //CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(victimHandle, "hndshkfa_swt", "gangs", 200.0f, 0, 0, 0, 0, 10);

    //bike        
    // 04C4: store_coords_to 1@ 2@ 3@ from_actor $VICTIM with_offset 0.0 2.0 0.0
    
    // 00A5: $BIKE = create_car 461 at 1@ 2@ 3@
    // 0918: set_car $BIKE engine_operation 1

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "Spawning pickpocket bike: " << 461 << std::endl;


    CVector bikePosition = position + CVector(0, 2, 0);

    auto bikeHandle = CleoFunctions::CREATE_CAR_AT(461, bikePosition.x, bikePosition.y, bikePosition.z);
    bike = Vehicles::TryCreateVehicle(bikeHandle);
    bike->AddBlip();

    CleoFunctions::SET_CAR_ENGINE_OPERATION(bikeHandle, 1);

    //driver    
    // 0129: $DRIVER = create_actor_pedtype 4 model 48 in_car $BIKE driverseat 

    auto driverSkin = Skins::GetRandomSkin(SkinGenre::SKIN_MALE, SkinGang::GANG_NONE);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "Spawning driver, skin: " << driverSkin.modelId << std::endl;

    // 0186: $MARKER = create_marker_above_car $BIKE
    auto driverHandle = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(bikeHandle, 4, driverSkin.modelId);
    driver = Peds::TryCreatePed(driverHandle);
    driver->AddBlip();

    // 04C4: store_coords_to 1@ 2@ 3@ from_actor $VICTIM with_offset 1.0 2.0 0.0
    // 009A: $GUNNER = create_actor_pedtype 23 model 48 at 1@ 2@ 3@
    
    // 01B2: give_actor $GUNNER weapon 22 ammo 10000
        
    // //why tf he walk backwards?                     
    // //0635: AS_actor $GUNNER aim_at_actor $VICTIM -1 ms
        
    // 07A3: AS_actor $GUNNER run_to_and_follow_actor $VICTIM wait_radius_between 0.5 and 1.0 

    CVector gunnerPosition = position + CVector(1, 2, 0);

    auto gunnerSkin = Skins::GetRandomSkin(SkinGenre::SKIN_MALE, SkinGang::GANG_NONE);
    auto gunnerHandle = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, gunnerSkin.modelId, gunnerPosition.x, gunnerPosition.y, gunnerPosition.z);
    gunner = Peds::TryCreatePed(gunnerHandle);
    gunner->AddBlip();

    CleoFunctions::GIVE_ACTOR_WEAPON(gunnerHandle, 22, 100);

    CleoFunctions::GOTO_CHAR_AIMING(gunnerHandle, victimHandle, 0.5, 1.0);
}

void BikePickpocket::FinishPickpocket()
{
    menuVSL->debug->AddLine("pickpocket finished");

    pickpocketInProgress = false;
    enteringBike = false;
    timeToSpawnPickpocket = BikePickpocket::m_TimeBetweenPickpockets;
}