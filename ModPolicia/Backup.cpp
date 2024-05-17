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
BACKUP_TYPE Backup::m_BackupType = BACKUP_TYPE::BACKUP_CHASE;
std::vector<BackupVehicle> Backup::m_DataBackupVehicles = {
    {596, 280, true, 22}, //LS
    {523, 193, false, 22}, //Bike
    {490, 286, true, 31}, //FBI
    {597, 281, true, 22}, //SF
    {598, 282, true, 22} //LV
};

void Backup::Update(int dt)
{
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
                Log::file << "making backup stop chase and move away from scene, removing vehicle from backup list" << std::endl;

                vehicle->MakePedsEnterVehicleAndLeaveScene();

                removeBackup.push_back(vehicle);
            }
            continue;
        }

         //if going to callout
        if(vehicle->actionStatus == ACTION_STATUS::CALLOUT_GOING_TO_CALLOUT)
        {
            auto distance = DistanceBetweenPoints(Mod::GetCarPosition(vehicle->hVehicle), vehicle->drivingTo);

            if(distance < 20)
            {
                vehicle->actionStatus = ACTION_STATUS::CALLOUT_ACTING_ON_CALLOUT;

                Log::file << "vehicle is now acting on callout" << std::endl;

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
                Log::file << "making backup stop acting on callouts and move away, removing vehicle from backup list" << std::endl;

                vehicle->MakePedsEnterVehicleAndLeaveScene();    

                removeBackup.push_back(vehicle);
            }
            continue;
        }

        /*
        if(vehicle->actionStatus == ACTION_STATUS::ACTION_NONE)
        {
            Log::file << "vehicle is ACTION_NONE, removing from backup list" << std::endl;
            Log::file << "current total vehiles " << m_BackupVehicles.size() << std::endl;

            vehicle->RemoveBlip();

            removeBackup.push_back(vehicle);
        }
        */
    }

    //remove vehicles
    for(auto vehicle : removeBackup)
    {
        auto it = std::find(m_BackupVehicles.begin(), m_BackupVehicles.end(), vehicle);
        m_BackupVehicles.erase(it);
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

void Backup::CallBackupCar(BackupVehicle backupVehicle)
{
    Log::file << "call backup vehicleModelId: " << backupVehicle.vehicleModelId << ", pedModelId: " << backupVehicle.pedModelId << std::endl;

    /*
    04D3: get_nearest_car_path_coords_from 0@ 1@ 2@ type 2 store_to 3@ 4@ 5@

    00A5: 6@ = create_car #COPCARLA at 3@ 4@ 5@
    0129: 10@ = create_actor_pedtype 23 model #LAPD1 in_car 6@ driverseat
            
    0397: enable_car 6@ siren 1
    00AD: set_car 6@ max_speed_to 50.0
    0423: set_car 6@ improved_handling_to 1.5
    00AE: set_car 6@ traffic_behaviour_to 2  

    07F8: car 6@ follow_car 8@ radius 8.0
    */


    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 100, 0, &x, &y, &z);

    float spawnX = 0, spawnY = 0, spawnZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(x, y, z, 2, &spawnX, &spawnY, &spawnZ);

    Log::file << "spawnX = " << spawnX << std::endl;
    Log::file << "spawnY = " << spawnY << std::endl;
    Log::file << "spawnZ = " << spawnZ << std::endl;

    auto car = CleoFunctions::CREATE_CAR_AT(backupVehicle.vehicleModelId, spawnX, spawnY, spawnZ);
    Log::file << "car = " << car << std::endl;

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

    Log::file << "creating driver model: " << backupVehicle.pedModelId << std::endl;

    int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 6, backupVehicle.pedModelId);
    auto pedDriver = Peds::TryCreatePed(driver);
    pedDriver->AddBlip(2);
    vehicle->hDriver = driver;

    Log::file << "driver = " << driver << std::endl;

    CleoFunctions::GIVE_ACTOR_WEAPON(driver, backupVehicle.weaponId, 10000);

    if(backupVehicle.hasPassenger)
    {
        int passenger = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(car, 6, backupVehicle.pedModelId, 0);
        auto pedPassenger = Peds::TryCreatePed(passenger);
        pedPassenger->AddBlip(2);
        vehicle->hPassengers.push_back(passenger);


        CleoFunctions::GIVE_ACTOR_WEAPON(passenger, backupVehicle.weaponId, 10000);
    }

    CleoFunctions::ENABLE_CAR_SIREN(car, true);
    CleoFunctions::SET_CAR_MAX_SPEED(car, 50.0f);
    //0423: set_car 6@ improved_handling_to 1.5
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(car, 2);
    
    if(m_BackupType == BACKUP_TYPE::BACKUP_CHASE)
    {
        Log::file << "set car " << car << " follow car " << Chase::m_ChasingPed->hVehicleOwned << std::endl;

        CleoFunctions::CAR_FOLLOR_CAR(car, Chase::m_ChasingPed->hVehicleOwned, 8.0f);
    }

    if(m_BackupType == BACKUP_TYPE::BACKUP_CALLOUT)
    {
        if(Callouts::m_Criminals.size() > 0)
        {
            Log::file << "make car drive to criminal" << std::endl;

            auto criminal = Callouts::m_Criminals[0];
            auto position = Mod::GetPedPosition(criminal->hPed);

            CleoFunctions::CAR_DRIVE_TO(car, position.x, position.y, position.z);

            vehicle->drivingTo = position;
        }
    }

    Log::file << "end call backup" << std::endl;
}

void Backup::CallBackupHeli()
{
    /*
    04C4: store_coords_to 0@ 1@ 2@ from_actor $PLAYER_ACTOR with_offset 0.0 0.0 50.0
    00A5: 3@ = create_car 497 at 0@ 1@ 2@
    0129: 7@ = create_actor_pedtype 23 model 280 in_car 3@ driverseat 
    0825: set_helicopter 3@ instant_rotor_start
    0918: set_car 3@ engine_operation 1 
    
    0186: 4@ = create_marker_above_car 3@

    02FF: show_text_3numbers GXT 'MPFX1' numbers 1 0 0 time 3000 flag 1

    04C4: store_coords_to 0@ 1@ 2@ from_actor $PLAYER_ACTOR with_offset 0.0 0.0 0.0

    073E: get_car_in_sphere 0@ 1@ 2@ radius 20.0 model -1 handle_as 5@

    if 056E: car 5@ defined
    then
        0186: 6@ = create_marker_above_car 5@
        
        0726: heli 3@ follow_actor -1 follow_car 5@ radius 15.0 
        03E5: show_text_box 'MPFX86' 
    end
    */
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float spawnX = 0, spawnY = 0, spawnZ = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 100, 100, 100, &spawnX, &spawnY, &spawnZ);

    auto heli = CleoFunctions::CREATE_CAR_AT(497, spawnX, spawnY, spawnZ);
    int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(heli, 23, 280);
    CleoFunctions::SET_HELICOPTER_INSTANT_ROTOR_START(heli);
    CleoFunctions::SET_CAR_ENGINE_OPERATION(heli, true);

    auto heliVehicle = Vehicles::TryCreateVehicle(heli);
    //heliVehicle->AddBlip(2);
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
        Log::file << "set heli " << heli << " follow ped " << Chase::m_ChasingPed->hPed << std::endl;

        CleoFunctions::HELI_FOLLOW(heli, Chase::m_ChasingPed->hPed, -1, 20.0f);
    }

    if(m_BackupType == BACKUP_TYPE::BACKUP_CALLOUT)
    {
        if(Callouts::m_Criminals.size() > 0)
        {
            Log::file << "make heli follow criminal" << std::endl;

            auto criminal = Callouts::m_Criminals[0];

            CleoFunctions::HELI_FOLLOW(heli, criminal->hPed, -1, 20.0f);
        }
    }

    

    CleoFunctions::WAIT(3000, []() {
        CleoFunctions::SHOW_TEXT_BOX("MPFX86");
    });
}