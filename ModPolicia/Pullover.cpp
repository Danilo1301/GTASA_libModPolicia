#include "Pullover.h"

#include "Widgets.h"
#include "Log.h"
#include "Peds.h"
#include "Vehicles.h"
#include "Chase.h"
#include "CleoFunctions.h"
#include "Input.h"
#include "Mod.h"

#include "windows/WindowTest.h"
#include "windows/WindowFrisk.h"
#include "windows/WindowPullover.h"

double Pullover::PULLOVER_MIN_DISTANCE_PED = 1;
double Pullover::PULLOVER_MIN_DISTANCE_VEHICLE = 2;
double Pullover::PULLOVER_MAX_DISTANCE = 30;

Ped* Pullover::m_PullingPed = NULL;
Vehicle* Pullover::m_PullingVehicle = NULL;
Ped* Pullover::m_ScorchingPed = NULL;
int Pullover::m_ScorchingPedSphere = 0;
int Pullover::m_ScorchingPedBlip = 0;
int Pullover::m_ScorchingToDpIndex = 0;

std::vector<Ped*> Pullover::m_PedsWaitingForScorchVehicle;

std::vector<CVector> Pullover::m_PoliceDepartmentPositions = {
    CVector(1536.1325, -1671.2093, 13.3828), //dp ls
    CVector(635.1006, -571.8163, 16.3359), //dp dillmore
    CVector(-1606.5406, 721.6789, 12.1554), //dp sf
    CVector(-211.8363, 978.0092, 19.3001), //dp fort carson
    CVector(2289.8784, 2425.8894, 10.8203) //dp lv
};

FRISK_TYPE Pullover::m_FriskType = FRISK_TYPE::FRISK_NONE;

void Pullover::Update(int dt)
{
    UpdateWidgetPress(dt);

    UpdateScorchingPed(dt);

    UpdatePullingPed(dt);
}

void Pullover::UpdateWidgetPress(int dt)
{
    if(m_PullingPed || m_PullingVehicle || Chase::m_ChasingPed)
        return;

    //if press horn
    if(Widgets::IsWidgetJustPressed(7))
    {   
        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        Log::file << "[Mod] Widget 7 has just been pressed" << std::endl;

        Log::file << "FindAimingPed" << std::endl;

        int aimingPed = FindAimingPed();

        Log::file << "aimingPed = " << aimingPed << std::endl;

        //if aiming ped
        if(aimingPed != -1)
        {   
            if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(aimingPed))
            {
                PullOverPed(aimingPed);
            } else {
                //TODO: get car of aiming ped, and pull the car (if possible)

                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX32", 0, 0, 0, 2000, 1); //nao encontrado
            }
            
        } else {
            //if not aiming anyone

            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor))
            {
                //if horn + 2
                if(Input::GetTouchIdState(2))
                {
                    TryPullOverCar();
                }
            } else {
                TryPullOverCar();
            }
        }
    }
}

void Pullover::UpdateScorchingPed(int dt)
{
    //update peds waiting for scorch vehicle

    std::vector<Ped*> removePeds;

    for(auto ped : m_PedsWaitingForScorchVehicle)
    {
        auto vehicle = Vehicles::GetVehicleByHandle(ped->waitingForScorchVehicleHandle);

        if(ped->scorchStatus == SCORCH_STATUS::WAITING_FOR_CAR)
        {
            auto carPos = Mod::GetCarPosition(vehicle->hVehicle);

            auto distance = DistanceBetweenPoints(carPos, vehicle->drivingTo);

            if(distance <= 5)
            {
                ped->shouldHandsup = false;

                CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(ped->hPed);

                ped->scorchStatus = SCORCH_STATUS::ENTERING_CAR;

                //05CA: AS_actor 3@ enter_car 7@ passenger_seat 1 time 10000
                CleoFunctions::ACTOR_ENTER_CAR_PASSENGER_SEAT(ped->hPed, vehicle->hVehicle, 10000, 1);

                continue;
            }
            continue;
        }

        if(ped->scorchStatus == SCORCH_STATUS::ENTERING_CAR)
        {
            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(ped->hPed))
            {
                ped->scorchStatus = SCORCH_STATUS::BEEING_DRIVEN_TO_FAR_AWAY;

                /*
                04C4: store_coords_to 0@ 1@ 2@ from_actor $PLAYER_ACTOR with_offset 200.0 0.0 0.0
                00A7: car 7@ drive_to 0@ 1@ 2@
                */

                float driveToX = 0, driveToY = 0, driveToZ = 0;
                CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(ped->hPed, 0, 200, 0, &driveToX, &driveToY, &driveToZ);
                CleoFunctions::CAR_DRIVE_TO(vehicle->hVehicle, driveToX, driveToY, driveToZ);

                continue;
            }
            continue;
        }

        if(ped->scorchStatus == SCORCH_STATUS::BEEING_DRIVEN_TO_FAR_AWAY)
        {
            int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

            auto distance = GetDistanceBetweenPedAndCar(playerActor, vehicle->hVehicle);

            if(distance > 80)
            {
                ped->scorchStatus = SCORCH_STATUS::NONE;
                ped->arrested = true;

                removePeds.push_back(ped);

                /*
                009B: destroy_actor 3@ 
                009B: destroy_actor 8@
                00A6: destroy_car 7@  
                */

                CleoFunctions::DESTROY_ACTOR(ped->hPed);
                CleoFunctions::DESTROY_ACTOR(vehicle->hDriver);
                CleoFunctions::DESTROY_CAR(vehicle->hVehicle);
            }

            continue;
        }
    }

    for(auto ped : removePeds)
    {
        auto it = std::find(m_PedsWaitingForScorchVehicle.begin(), m_PedsWaitingForScorchVehicle.end(), ped);
        m_PedsWaitingForScorchVehicle.erase(it);
    }

    //

    if(!m_ScorchingPed) return;
    
    if(!CleoFunctions::ACTOR_DEFINED(m_ScorchingPed->hPed) || CleoFunctions::ACTOR_DEAD(m_ScorchingPed->hPed))
    {
        Log::file << "Scorching ped became undefined or is now dead" << std::endl;

        m_ScorchingPed->scorchStatus = SCORCH_STATUS::NONE;
        m_ScorchingPed->RemoveBlip();
        m_ScorchingPed = NULL;
        
        CleoFunctions::DESTROY_SPHERE(m_ScorchingPedSphere);
        m_ScorchingPedSphere = 0;

        CleoFunctions::DISABLE_MARKER(m_ScorchingPedBlip);
        m_ScorchingPedBlip = 0;

        return;
    }

    CVector dpPosition = m_PoliceDepartmentPositions[m_ScorchingToDpIndex];

    auto pedPos = Mod::GetPedPosition(m_ScorchingPed->hPed);

    auto distance = DistanceBetweenPoints(dpPosition, pedPos);

    if(distance < 3.0f)
    {
        /*
        01C2: remove_references_to_actor 4@ 
        009B: destroy_actor 4@ 
        03BD: destroy_sphere 7@
        */

        CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(m_ScorchingPed->hPed);

        m_ScorchingPed->scorchStatus = SCORCH_STATUS::NONE;
        m_ScorchingPed->arrested = true;

        CleoFunctions::DESTROY_ACTOR(m_ScorchingPed->hPed);
        m_ScorchingPed = NULL;
        m_PullingPed = NULL;
        m_PullingVehicle = NULL;

        CleoFunctions::DESTROY_SPHERE(m_ScorchingPedSphere);
        m_ScorchingPedSphere = 0;

        CleoFunctions::DISABLE_MARKER(m_ScorchingPedBlip);
        m_ScorchingPedBlip = 0;
    }
}

void Pullover::UpdatePullingPed(int dt)
{
    if(!Pullover::m_PullingPed) return;
    
    auto ped = Pullover::m_PullingPed;
    auto hVehicle = ped->hVehicleOwned;
    auto vehicle = Vehicles::GetVehicleByHandle(hVehicle);

    if(vehicle)
    {
        int driver = CleoFunctions::GET_DRIVER_OF_CAR(vehicle->hVehicle);

        //someone just fucking stole his vehicle
        if(driver != ped->hPed)
        {
            CleoFunctions::EXIT_CAR_AS_ACTOR(driver);
        }
    }
}

void Pullover::PullOverPed(int hPed)
{
    Log::file << "pull over ped" << std::endl;

    /*
    

    float range = 4.0f;
    if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) range = 7.0f;

    float x = 0.0f, y = 0.0f, z = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0.0f, range, 0.0f, &x, &y, &z);

    int randomChar = CleoFunctions::GET_RANDOM_CHAR_IN_SPHERE(x, y, z, 4.0f, true, true, true);

    if(randomChar == -1)
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX32", 0, 0, 0, 2000, 1); //nao encontrado
        return;
    }
    */

    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor))
        CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "CopTraf_Stop", "POLICE", 4.0f, 0, 0, 0, 0, -1);

    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX31", 0, 0, 0, 2000, 1); //parado!

    m_PullingPed = Peds::TryCreatePed(hPed);
    m_PullingPed->UpdateInventory();

    m_PullingPed->AddBlip();

    m_PullingPed->shouldHandsup = true;

    if(m_PullingPed->hVehicleOwned > 0) m_PullingVehicle = Vehicles::GetVehicleByHandle(m_PullingPed->hVehicleOwned);

    CleoFunctions::WAIT(2000, []() {
        WindowPullover::CreatePullingPed();
    });
}

void Pullover::TryPullOverCar()
{
    Log::file << "try pull over car " << std::endl;

    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float range = 4.0f;
    if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) range = 7.0f;

    float x = 0.0f, y = 0.0f, z = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0.0f, range, 0.0f, &x, &y, &z);

    int randomCar = CleoFunctions::GET_CAR_IN_SPHERE(x, y, z, 5.0f, -1);

    //if no car is found
    if(randomCar <= 0)
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX32", 0, 0, 0, 2000, 1); //nao encontrado
        return;
    }

    int driver = CleoFunctions::GET_DRIVER_OF_CAR(randomCar);
    
    //if has no driver
    if(driver <= 0)
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX32", 0, 0, 0, 2000, 1); //nao encontrado
        return;
    }
    
    if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor))
        CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "CopTraf_Stop", "POLICE", 4.0f, 0, 0, 0, 0, -1);

    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX31", 0, 0, 0, 2000, 1); //parado!

    /*
    float carX = 0.0f, carY = 0.0f, carZ = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(randomCar, 0.0f, 0.0f, 0.0f, &carX, &carY, &carZ);
    Log::file << "carX: " << carX << std::endl;
    Log::file << "carY: " << carY << std::endl;
    Log::file << "carZ: " << carZ << std::endl;
    */

    auto vehicle = Vehicles::TryCreateVehicle(randomCar);
    vehicle->UpdateInventory();
    vehicle->AddBlip();

    auto ped = Peds::TryCreatePed(driver);
    ped->UpdateInventory();
    ped->hVehicleOwned = vehicle->hVehicle;
    ped->AddBlip();

    if(vehicle->HasIlegalStuff() || vehicle->isStolen || ped->isWanted)
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX74", 0, 0, 0, 3000, 1); //apreendeu fuga!

        Chase::MakeCarStartRunning(vehicle, ped);
        return;
    }

    m_PullingVehicle = vehicle;
    m_PullingPed = ped;

    CleoFunctions::CAR_TURN_OFF_ENGINE(randomCar);
    
    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX63", 0, 0, 0, 3000, 1); //chegue mais perto

    //wait to get closer to the car
    CleoFunctions::AddWaitForFunction([playerActor, randomCar] () {
        
        if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) return false;

        auto distance = GetDistanceBetweenPedAndCar(playerActor, randomCar);

        Log::file << "distance from car: " << distance << std::endl;

        if(distance < PULLOVER_MIN_DISTANCE_VEHICLE) return true;
        if(distance > PULLOVER_MAX_DISTANCE) return true;

        return false;
    },
    [playerActor, randomCar] () {
        auto distance = GetDistanceBetweenPedAndCar(playerActor, randomCar);

        if(distance <= PULLOVER_MIN_DISTANCE_VEHICLE)
        {
            Log::file << "Create pulling car menu" << std::endl;
            WindowPullover::CreatePullingCar();
        } else {
            Log::file << "Car is too far away!" << std::endl;
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX56", 0, 0, 0, 3000, 1); //muito longe
        }
    });
}

int Pullover::FindAimingPed()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    for(int ped = 0; ped < 35584; ped++)
    {
        //Log::file << "ped " << ped << ", playerActor " << playerActor << std::endl;


         
        //Log::file << "test1" << std::endl;
        //CleoFunctions::PLAYER_AIMING_AT_ACTOR(0, ped);

        //Log::file << "test2" << std::endl;
        //CleoFunctions::PLAYER_AIMING_AT_ACTOR(playerActor, ped);

        //Log::file << "test3" << std::endl;
        if(CleoFunctions::PLAYER_AIMING_AT_ACTOR(0, ped))
        {
            return ped;
        }
    }
    return -1;
}

void Pullover::FriskPed()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "hndshkfa_swt", "gangs", 2.0f, 0, 0, 0, 0, -1);

    m_PullingPed->UpdateInventory();

    CleoFunctions::WAIT(3500, []() {
        WindowFrisk::Create();
    });
}

void Pullover::FriskVehicle()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "hndshkfa_swt", "gangs", 2.0f, 0, 0, 0, 0, -1);

    CleoFunctions::WAIT(3500, []() {
        WindowFrisk::CreateFriskCar();
    });
}

void Pullover::FreePed()
{
    //CleoFunctions::SET_PLAYER_CAN_MOVE(m_PullingPed->hPed, true);

    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX55", 0, 0, 0, 3000, 1); //liberado

    m_PullingPed->RemoveBlip();
    m_PullingPed->shouldHandsup = false;

    Log::file << "Remove references to actor " << m_PullingPed->hPed << std::endl;
    CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(m_PullingPed->hPed);
        
    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(m_PullingPed->hPed, "hndshkfa_swt", "gangs", 200.0f, 0, 0, 0, 0, 10);
            
    m_PullingPed = NULL;
}

void Pullover::MakePedWait()
{
    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX80", 0, 0, 0, 3000, 1); //aguarde no local

    m_PullingPed->RemoveBlip();
    if(m_PullingPed->hVehicleOwned > 0) {
        m_PullingVehicle = NULL;
        Vehicles::GetVehicleByHandle(m_PullingPed->hVehicleOwned)->RemoveBlip();
    }
    
    m_PullingPed = NULL;

}

void Pullover::FreeVehicle()
{
    m_PullingPed->shouldHandsup = false;

    CleoFunctions::ENTER_CAR_AS_DRIVER_AS_ACTOR(m_PullingPed->hPed,  m_PullingPed->hVehicleOwned, 20000);

    m_PullingVehicle->RemoveBlip();
    m_PullingPed->RemoveBlip();

    m_PullingPed = NULL;
    m_PullingVehicle = NULL;
}

void Pullover::AskPedToLeaveCar(Ped* ped)
{
    /*
    0633: AS_actor 21@ exit_car
    wait 1000 
    0812: AS_actor 21@ perform_animation "handsup" IFP "PED" framedelta 4.0 loopA 0 lockX 0 lockY 0 lockF 1 time -1 // versionB 
    */
    CleoFunctions::EXIT_CAR_AS_ACTOR(ped->hPed);
    
    CleoFunctions::WAIT(1000, [ped]() {
        ped->shouldHandsup = true;
    });

    CleoFunctions::WAIT(2000, []() {
        WindowPullover::CreatePullingPed();
    });
}

void Pullover::StartScorchingPed(Ped* ped)
{
    Pullover::m_PullingPed = NULL;
    Pullover::m_PullingVehicle = NULL;

    Pullover::m_ScorchingPed = ped;

    ped->scorchStatus = SCORCH_STATUS::BEEING_SCORCHED;

    if(ped->hVehicleOwned > 0)
    {
        Vehicles::GetVehicleByHandle(ped->hVehicleOwned)->RemoveBlip();
    }

    ped->hVehicleOwned = 0;
    
    Log::file << "Conduzir para a DP" << std::endl;

    /*
    01C2: remove_references_to_actor 4@

    07AF: 6@ = player $PLAYER_CHAR group 

    //062F: 6@ = create_group_type 0 //is this really encessary    
    0630: put_actor $PLAYER_ACTOR in_group 4@ as_leader 
    0631: put_actor 4@ in_group 6@
    */

    ped->shouldHandsup = false;

    CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(ped->hPed);
        
    auto playerGroup = CleoFunctions::GET_PLAYER_GROUP(0);

    Log::file << "playerGroup = " << playerGroup << std::endl;

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    /*
    CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(playerActor, ped->hPed);
    CleoFunctions::PUT_ACTOR_IN_GROUP(ped->hPed, playerGroup);
    */
    CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(ped->hPed, playerActor);
    CleoFunctions::PUT_ACTOR_IN_GROUP(playerGroup, ped->hPed);

    Log::file << "create sphere" << std::endl;

    Pullover::m_ScorchingToDpIndex = GetClosestPoliceDepartment();
    CVector dpPosition = m_PoliceDepartmentPositions[Pullover::m_ScorchingToDpIndex];

    Pullover::m_ScorchingPedSphere = CleoFunctions::CREATE_SPHERE(dpPosition.x, dpPosition.y, dpPosition.z, 3.0);
    Log::file << "sphere = " << Pullover::m_ScorchingPedSphere << std::endl;

    Pullover::m_ScorchingPedBlip = CleoFunctions::CreateMarker(dpPosition.x, dpPosition.y, dpPosition.z, 0, 3, 3);
    Log::file << "blip = " << Pullover::m_ScorchingPedBlip << std::endl;
}

void Pullover::TeleportPedToPrision(Ped* ped)
{
    Log::file << "TeleportPedToPrision" << std::endl;

    Pullover::m_PullingPed = NULL;
    Pullover::m_PullingVehicle = NULL;

    ped->arrested = true;

    Log::file << "destroy actor" << std::endl;

    CleoFunctions::DESTROY_ACTOR(ped->hPed);

    Log::file << "fade" << std::endl;

    CleoFunctions::FADE(500, 1);
}

void Pullover::CallVehicleToScorchPed(Ped* ped)
{
    Log::file << "Call vehicle to scorch ped" << std::endl;

    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX85", 0, 0, 0, 2000, 1); //solicito viatura

    Pullover::m_PullingPed = NULL;
    Pullover::m_PullingVehicle = NULL;

    m_PedsWaitingForScorchVehicle.push_back(ped);

    ped->scorchStatus = SCORCH_STATUS::WAITING_FOR_CAR;

    if(ped->hVehicleOwned > 0) Vehicles::GetVehicleByHandle(ped->hVehicleOwned)->RemoveBlip();
    ped->hVehicleOwned = 0;
    
    /*
    04C4: store_coords_to 0@ 1@ 2@ from_actor $PLAYER_ACTOR with_offset 0.0 0.0 0.0

    009A: 3@ = create_actor_pedtype 23 model 193 at 0@ 1@ 2@ //ped

    04C4: store_coords_to 0@ 1@ 2@ from_actor $PLAYER_ACTOR with_offset 100.0 0.0 0.0
    04D3: get_nearest_car_path_coords_from 0@ 1@ 2@ type 2 store_to 4@ 5@ 6@

    00A5: 7@ = create_car 596 at 4@ 5@ 6@
    0397: enable_car 7@ siren 1
    //0519: set_car 7@ locked 0 //wrong opcode
    020A: set_car 0@ door_status_to 1 
    
    00AE: set_car 7@ traffic_behaviour_to 2
    0186: 9@ = create_marker_above_car 7@
        
    0129: 8@ = create_actor_pedtype 23 model 280 in_car 7@ driverseat

    04C4: store_coords_to 0@ 1@ 2@ from_actor 3@ with_offset 0.0 0.0 0.0

    00A7: car 7@ drive_to 0@ 1@ 2@
    */
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float trySpawnCarX = 0, trySpawnCarY = 0, trySpawnCarZ = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 100, 0, &trySpawnCarX, &trySpawnCarY, &trySpawnCarZ);

    float spawnCarX = 0, spawnCarY = 0, spawnCarZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(trySpawnCarX, trySpawnCarY, trySpawnCarZ, 2, &spawnCarX, &spawnCarY, &spawnCarZ);

    int car = CleoFunctions::CREATE_CAR_AT(596, spawnCarX, spawnCarY, spawnCarZ);
    auto vehicle = Vehicles::TryCreateVehicle(car);
    CleoFunctions::ENABLE_CAR_SIREN(car, true);
    //CleoFunctions::SET_CAR_LOCKED(car, false);
    CleoFunctions::SET_CAR_DOOR_STATUS(car, 1);
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(car, 2);
    vehicle->AddBlip();
    
    ped->waitingForScorchVehicleHandle = car;

    int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 23, 280);

    vehicle->hDriver = driver;

    float actorX = 0, actorY = 0, actorZ = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(ped->hPed, 0, 0, 0, &actorX, &actorY, &actorZ);

    float driveToX = 0, driveToY = 0, driveToZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(actorX, actorY, actorZ, 2, &driveToX, &driveToY, &driveToZ);

    vehicle->drivingTo = CVector(driveToX, driveToY, driveToZ);

    CleoFunctions::CAR_DRIVE_TO(car, driveToX, driveToY, driveToZ);
}

double Pullover::GetDistanceBetweenPedAndCar(int hChar, int hVehicle)
{
    float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
    CleoFunctions::GET_CHAR_COORDINATES(hChar, &playerX, &playerY, &playerZ);

    float carX = 0.0f, carY = 0.0f, carZ = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(hVehicle, 0.0f, 0.0f, 0.0f, &carX, &carY, &carZ);

    auto distance = DistanceBetweenPoints(CVector(playerX, playerY, playerZ), CVector(carX, carY, carZ));
    return distance;
}

double Pullover::GetDistanceBetweenPeds(int hChar, int hChar2)
{
    float x1 = 0.0f, y1 = 0.0f, z1 = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(hChar, 0, 0, 0, &x1, &y1, &z1);

    float x2 = 0.0f, y2 = 0.0f, z2 = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(hChar2, 0, 0, 0, &x2, &y2, &z2);

    auto distance = DistanceBetweenPoints(CVector(x1, y1, z1), CVector(x2, y2, z2));
    return distance;
}

int Pullover::GetClosestPoliceDepartment()
{
    int closest = -1;
    double closestDistance = INFINITY;

    for(size_t i = 0; i < m_PoliceDepartmentPositions.size(); i++)
    {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &playerX, &playerY, &playerZ);

        auto dpPosition = m_PoliceDepartmentPositions[i];

        auto distance = DistanceBetweenPoints(dpPosition, CVector(playerX, playerY, playerZ));

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closest = (int)i;
        }
    }

    return closest;
}