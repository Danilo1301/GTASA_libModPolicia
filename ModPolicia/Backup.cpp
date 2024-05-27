#include "Backup.h"

#include "Chase.h"
#include "Callouts.h"
#include "Log.h"
#include "CleoFunctions.h"
#include "Mod.h"
#include "Widgets.h"
#include "Vehicles.h"
#include "Peds.h"

#include "windows/WindowBackup.h"

std::vector<Vehicle*> Backup::m_BackupVehicles;
std::vector<Ped*> Backup::m_BackupPeds;
BACKUP_TYPE Backup::m_BackupType = BACKUP_TYPE::BACKUP_CHASE;
std::vector<BackupVehicle> Backup::m_DataBackupVehicles = {
    {596, 280, 2, 4, 22}, //LS
    {523, 284, 1, 2, 22}, //Bike
    {490, 286, 4, 4, 31}, //FBI
    {597, 281, 2, 4, 22}, //SF
    {598, 282, 2, 4, 22}, //LV
    {599, 283, 4, 4, 22} //Ranger
};
std::vector<int> Backup::m_DataBackupWeapons = {22, 31, 24, 25};

void Backup::Update(int dt)
{
    UpdateBackupPeds(dt);
    UpdateBackupVehiclesActionStatus(dt);

    UpdateChaseBackup(dt);

    UpdateCalloutBackup(dt);
    
    //if backup window is open (try to close)
    if(WindowBackup::m_Window)
    {
        //if not chasing, remove window
        if(m_BackupType == BACKUP_TYPE::BACKUP_CHASE && !Chase::m_ChasingPed) WindowBackup::Remove();

        //if not on callout, remove window
        if(m_BackupType == BACKUP_TYPE::BACKUP_CALLOUT && !Callouts::IsOnCallout()) WindowBackup::Remove();
    }
}

void Backup::UpdateBackupVehiclesActionStatus(int dt)
{
    std::vector<Vehicle*> removeBackup;
    for(auto vehicle : m_BackupVehicles)
    {
        //if backup is chasing
        if(vehicle->actionStatus == ACTION_STATUS::CHASE_CHASING)
        {
            if(!Chase::m_ChasingPed)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "making backup stop chase and move away from scene, removing vehicle from backup list" << std::endl;

                vehicle->MakePedsEnterVehicleAndLeaveScene();

                vehicle->RemoveDriverAndPassengersBlip();

                removeBackup.push_back(vehicle);
            }
            continue;
        }

         //if going to callout
        if(vehicle->actionStatus == ACTION_STATUS::CALLOUT_GOING_TO_CALLOUT)
        {
            if(Callouts::m_Criminals.size() == 0)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "going to callout but there are no criminals" << std::endl;
                Log::Level(LOG_LEVEL::LOG_BOTH) << "set to acting on callout" << std::endl;

                vehicle->actionStatus = ACTION_STATUS::CALLOUT_ACTING_ON_CALLOUT;

                continue;
            }

            auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);
            auto criminal = Callouts::GetClosestCriminal(vehiclePosition);
            auto criminalPosition = Mod::GetPedPosition(criminal->hPed);

            if(DistanceBetweenPoints(criminalPosition, vehicle->drivingTo) > 3)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "criminal is far away from old position" << std::endl;

                vehicle->drivingTo = criminalPosition;

                CleoFunctions::CAR_DRIVE_TO(vehicle->hVehicle, criminalPosition.x, criminalPosition.y, criminalPosition.z);
            }

            auto distance = DistanceBetweenPoints(Mod::GetCarPosition(vehicle->hVehicle), vehicle->drivingTo);

            if(distance < 20)
            {
                vehicle->actionStatus = ACTION_STATUS::CALLOUT_ACTING_ON_CALLOUT;

                Log::Level(LOG_LEVEL::LOG_BOTH) << "vehicle is now acting on callout" << std::endl;

                //05E2: AS_actor $POLICE kill_actor $CRIMINAL

                for(auto criminal : Callouts::m_Criminals)
                {
                    CleoFunctions::KILL_ACTOR(vehicle->hDriver, criminal->hPed);

                    for(auto passenger : vehicle->hPassengers)
                        CleoFunctions::KILL_ACTOR(passenger, criminal->hPed);
                }
            }

            continue;
        }

        //if is on callout and callout ends
        if(vehicle->actionStatus == ACTION_STATUS::CALLOUT_ACTING_ON_CALLOUT)
        {
            if(!Callouts::IsOnCallout())
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "making backup stop acting on callouts and move away, removing vehicle from backup list" << std::endl;

                vehicle->MakePedsEnterVehicleAndLeaveScene();    

                vehicle->RemoveDriverAndPassengersBlip();

                removeBackup.push_back(vehicle);
            }
            continue;
        }
    }

    //remove vehicles
    for(auto vehicle : removeBackup)
    {
        auto it = std::find(m_BackupVehicles.begin(), m_BackupVehicles.end(), vehicle);
        m_BackupVehicles.erase(it);
    }
}

void Backup::UpdateBackupPeds(int dt)
{
    std::vector<Ped*> toRemove;
    for(auto ped : m_BackupPeds)
    {
        if(!Mod::IsActorAliveAndDefined(ped->hPed))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "backup ped is not defined/alived anymore, removing blip" << std::endl;

            ped->RemoveBlip();

            toRemove.push_back(ped);
            continue;
        }
    }

    //remove peds
    for(auto item : toRemove)
    {
        auto it = std::find(m_BackupPeds.begin(), m_BackupPeds.end(), item);
        m_BackupPeds.erase(it);
    }
}

void Backup::UpdateChaseBackup(int dt)
{
    //show backup button
    if(Chase::m_ChasingPed)
    {
        if(!WindowBackup::m_Window)
        {
            if(Widgets::IsWidgetJustPressed(37)) //green button
            {
                m_BackupType = BACKUP_TYPE::BACKUP_CHASE;
                WindowBackup::Create();
            }
        }
    }
}

void Backup::UpdateCalloutBackup(int dt)
{
    //show backup button
    if(Callouts::IsOnCallout() && Callouts::m_Criminals.size() > 0)
    {
        if(!WindowBackup::m_Window)
        {
            if(Widgets::IsWidgetJustPressed(37)) //green button
            {
                m_BackupType = BACKUP_TYPE::BACKUP_CALLOUT;
                WindowBackup::Create();
            }
        }
    }
}

void Backup::CallBackupCar(BackupVehicle* backupVehicle)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "call backup vehicleModelId: " << backupVehicle->vehicleModelId << ", pedModelId: " << backupVehicle->pedModelId << std::endl;

    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 100, 0, &x, &y, &z);

    float spawnX = 0, spawnY = 0, spawnZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(x, y, z, 2, &spawnX, &spawnY, &spawnZ);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "spawnX = " << spawnX << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "spawnY = " << spawnY << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "spawnZ = " << spawnZ << std::endl;

    auto car = CleoFunctions::CREATE_CAR_AT(backupVehicle->vehicleModelId, spawnX, spawnY, spawnZ);
    Log::Level(LOG_LEVEL::LOG_BOTH) << "car = " << car << std::endl;

    auto vehicle = Vehicles::TryCreateVehicle(car);
    //vehicle->AddBlip(2);

    if(m_BackupType == BACKUP_TYPE::BACKUP_CHASE)
    {
        vehicle->actionStatus = ACTION_STATUS::CHASE_CHASING;
    } else if (m_BackupType == BACKUP_TYPE::BACKUP_CALLOUT)
    {
        vehicle->actionStatus = ACTION_STATUS::CALLOUT_GOING_TO_CALLOUT;
    }

    m_BackupVehicles.push_back(vehicle);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "creating driver model: " << backupVehicle->pedModelId << std::endl;

    //23 = special
    //6 = cop
    //to fix a issue
    int type = 6;
    if(backupVehicle->pedModelId == 284) type = 23;

    int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, type, backupVehicle->pedModelId);
    auto pedDriver = Peds::TryCreatePed(driver);
    pedDriver->AddBlip(2);
    vehicle->hDriver = driver;
    m_BackupPeds.push_back(pedDriver);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "driver = " << driver << std::endl;

    CleoFunctions::GIVE_ACTOR_WEAPON(driver, backupVehicle->weaponId, 10000);

    for(int i = 0; i < backupVehicle->numPeds - 1; i++)
    {
        int passenger = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(car, type, backupVehicle->pedModelId, i);
        auto pedPassenger = Peds::TryCreatePed(passenger);
        pedPassenger->AddBlip(2);
        vehicle->hPassengers.push_back(passenger);
        m_BackupPeds.push_back(pedPassenger);

        CleoFunctions::GIVE_ACTOR_WEAPON(passenger, backupVehicle->weaponId, 10000);
    }

    CleoFunctions::ENABLE_CAR_SIREN(car, true);
    CleoFunctions::SET_CAR_MAX_SPEED(car, 50.0f);
    //0423: set_car 6@ improved_handling_to 1.5
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(car, 2);
    
    if(m_BackupType == BACKUP_TYPE::BACKUP_CHASE)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "set car " << car << " follow car " << Chase::m_ChasingPed->hVehicleOwned << std::endl;

        CleoFunctions::CAR_FOLLOR_CAR(car, Chase::m_ChasingPed->hVehicleOwned, 8.0f);
    }

    if(m_BackupType == BACKUP_TYPE::BACKUP_CALLOUT)
    {
        if(Callouts::m_Criminals.size() > 0)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "make car drive to closest criminal" << std::endl;
            
            auto criminal = Callouts::GetClosestCriminal(CVector(spawnX, spawnY, spawnZ));
            auto position = Mod::GetPedPosition(criminal->hPed);

            CleoFunctions::CAR_DRIVE_TO(car, position.x, position.y, position.z);

            vehicle->drivingTo = position;
        }
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "end call backup" << std::endl;
}

void Backup::CallBackupHeli()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float spawnX = 0, spawnY = 0, spawnZ = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 100, 100, 100, &spawnX, &spawnY, &spawnZ);

    auto heli = CleoFunctions::CREATE_CAR_AT(497, spawnX, spawnY, spawnZ);

    int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(heli, 23, 280);
    auto driverPed = Peds::TryCreatePed(driver);

    CleoFunctions::SET_HELICOPTER_INSTANT_ROTOR_START(heli);
    CleoFunctions::SET_CAR_ENGINE_OPERATION(heli, true);

    auto heliVehicle = Vehicles::TryCreateVehicle(heli);
    heliVehicle->AddBlip(2);
    heliVehicle->hDriver = driver;

    if(m_BackupType == BACKUP_TYPE::BACKUP_CHASE)
    {
        heliVehicle->actionStatus = ACTION_STATUS::CHASE_CHASING;
    } else if (m_BackupType == BACKUP_TYPE::BACKUP_CALLOUT)
    {
        heliVehicle->actionStatus = ACTION_STATUS::CALLOUT_ACTING_ON_CALLOUT;
    }

    m_BackupVehicles.push_back(heliVehicle);

    if(m_BackupType == BACKUP_TYPE::BACKUP_CHASE)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "set heli " << heli << " follow ped " << Chase::m_ChasingPed->hPed << std::endl;

        CleoFunctions::HELI_FOLLOW(heli, Chase::m_ChasingPed->hPed, -1, 20.0f);
    }

    if(m_BackupType == BACKUP_TYPE::BACKUP_CALLOUT)
    {
        if(Callouts::m_Criminals.size() > 0)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "make heli follow criminal" << std::endl;

            auto criminal = Callouts::m_Criminals[0];

            CleoFunctions::HELI_FOLLOW(heli, criminal->hPed, -1, 20.0f);
        }
    }

    

    CleoFunctions::WAIT(3000, []() {
        CleoFunctions::SHOW_TEXT_BOX("MPFX86");
    });
}