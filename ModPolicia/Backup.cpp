#include "Backup.h"

#include "Chase.h"
#include "Callouts.h"
#include "Log.h"
#include "CleoFunctions.h"
#include "Mod.h"
#include "Widgets.h"
#include "Vehicles.h"
#include "Peds.h"
#include "SoundSystem.h"

#include "windows/WindowBackup.h"

std::vector<Vehicle*> Backup::m_BackupVehicles;
std::vector<Ped*> Backup::m_BackupPeds;
BACKUP_TYPE Backup::m_BackupType = BACKUP_TYPE::BACKUP_CHASE;
std::vector<BackupVehicle> Backup::m_DataBackupVehicles = {
    {596, 280, 2, 4, 22, "voices/REQUEST_BACKUP_LS_", "voices/UNIT_RESPONDING_DISPATCH_"}, //LS
    {523, 284, 1, 2, 22, "voices/REQUEST_BACKUP_BIKE_", "voices/UNIT_RESPONDING_DISPATCH_"}, //Bike
    {490, 286, 4, 4, 31, "voices/REQUEST_BACKUP_FBI_", "voices/UNIT_RESPONDING_DISPATCH_"}, //FBI
    {597, 281, 2, 4, 22, "voices/REQUEST_BACKUP_SF_", "voices/UNIT_RESPONDING_DISPATCH_"}, //SF
    {598, 282, 2, 4, 22, "voices/REQUEST_BACKUP_LV_", "voices/UNIT_RESPONDING_DISPATCH_"}, //LV
    {599, 283, 2, 4, 22, "voices/REQUEST_BACKUP_RANGER_", "voices/UNIT_RESPONDING_DISPATCH_"}, //Ranger
    {497, 280, 1, 1, 22, "voices/REQUEST_BACKUP_HELI_", "voices/HELI_APPROACHING_DISPATCH_"}, //Polmav
    {601, 285, 2, 4, 31, "voices/REQUEST_BACKUP_SWAT_", "voices/UNIT_RESPONDING_DISPATCH_"} //SWAT
};
std::vector<int> Backup::m_DataBackupWeapons = {22, 31, 24, 25};

AudioStream* Backup::m_RequestBackupAudio = NULL;
bool Backup::m_WaitingToRespondDispatch = false;
std::string Backup::m_IncomingBackupSoundName = "";

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

    //update audio
    if(m_RequestBackupAudio != NULL)
    {
        auto state = m_RequestBackupAudio->GetState();

        if(state != 1)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "backup audio was playing but it now stopped" << std::endl;

            m_RequestBackupAudio->Destroy();
            m_RequestBackupAudio = NULL;
        }
    }

    if(m_WaitingToRespondDispatch)
    {
        if(!m_RequestBackupAudio)
        {
            m_WaitingToRespondDispatch = false;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "responding dispatch in 1 second" << std::endl;

            CleoFunctions::WAIT(1000, []() {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "playing audio: " << m_IncomingBackupSoundName << std::endl;

                SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation(m_IncomingBackupSoundName, false);
            });

            //SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/HELI_APPROACHING_DISPATCH_", false);
            //SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/UNIT_RESPONDING_DISPATCH_", false);
        }
    }
}

void Backup::UpdateBackupVehiclesActionStatus(int dt)
{
    
}

void Backup::UpdateBackupPeds(int dt)
{
    float distanceExitCar = 10.0f;
    float distanceEnterCar = 30.0f;

    std::vector<Ped*> pedsToRemove;

    for(auto ped : m_BackupPeds)
    {
        if(!Mod::IsActorAliveAndDefined(ped->hPed))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "backup ped is not defined/alived anymore, removing blip" << std::endl;

            ped->RemoveBlip();

            pedsToRemove.push_back(ped);
            continue;
        }

        //remove cops if the are no criminals anymore and they are far from player
        if(Callouts::m_Criminals.size() == 0)
        {
            auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
            auto playerPosition = Mod::GetPedPosition(playerActor);
            auto pedPosition = Mod::GetPedPosition(ped->hPed);
            auto distanceFromPlayer = DistanceBetweenPoints(pedPosition, playerPosition);

            if(distanceFromPlayer > 200.0f)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "backup ped " << ped->hPed << " is too far away, destroying" << std::endl;

                CleoFunctions::DESTROY_ACTOR(ped->hPed);
                pedsToRemove.push_back(ped);
            }
        }
    }

    //remove peds 1
    for(auto item : pedsToRemove)
    {
        auto it = std::find(m_BackupPeds.begin(), m_BackupPeds.end(), item);
        m_BackupPeds.erase(it);
    }
    pedsToRemove.clear();
    
    //

    std::vector<Vehicle*> vehiclesToRemove;
    for(auto vehicle : m_BackupVehicles)
    {
        if(!CleoFunctions::CAR_DEFINED(vehicle->hVehicle))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "backup vehicle is not defined anymore, removing" << std::endl;

            vehicle->RemoveBlip();
            vehicle->RemoveDriverAndPassengersBlip();

            vehiclesToRemove.push_back(vehicle);
            continue;
        }

        //remove vehicles if the are no criminals anymore and they are far from player
        if(Callouts::m_Criminals.size() == 0)
        {
            auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
            auto playerPosition = Mod::GetPedPosition(playerActor);
            auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);
            auto distanceFromPlayer = DistanceBetweenPoints(vehiclePosition, playerPosition);

            if(distanceFromPlayer > 200.0f)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "backup vehicle " << vehicle->hVehicle << " is too far away, destroying" << std::endl;

                CleoFunctions::DESTROY_CAR(vehicle->hVehicle);
                vehiclesToRemove.push_back(vehicle);
            }
        }
    }

    //remove vehicles
    for(auto item : vehiclesToRemove)
    {
        auto it = std::find(m_BackupVehicles.begin(), m_BackupVehicles.end(), item);
        m_BackupVehicles.erase(it);
    }

    //

    for(auto vehicle : m_BackupVehicles)
    {
        auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);
        auto criminal = Callouts::GetClosestCriminal(vehiclePosition);

        if(criminal)
        {
            auto criminalPosition = Mod::GetPedPosition(criminal->hPed);
            auto distanceFromCriminal = DistanceBetweenPoints(vehiclePosition, criminalPosition);

            //if criminal is 30m far away from ped
            if(distanceFromCriminal > distanceEnterCar)
            {
                if(vehicle->actionStatus == ACTION_STATUS::ACTION_NONE)
                {
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "vehicle status is NONE" << std::endl;

                    auto allPassengersInSeat = vehicle->IsAllDriverAndPassengersInsideCar();

                    if(allPassengersInSeat)
                    {
                        Log::Level(LOG_LEVEL::LOG_BOTH) << "vehicle status was NONE, but is now driving to criminal" << std::endl;

                        vehicle->actionStatus = ACTION_STATUS::GOING_TO_SUSPECT;
                        vehicle->goingToPed = criminal->hPed;
                        //vehicle->followingCar = 0;
                        vehicle->drivingTo = CVector(0, 0, 0);
                    }
                }
            }

            //if distance is less than 10m
            if(distanceFromCriminal < distanceExitCar)
            {
                if(vehicle->actionStatus == ACTION_STATUS::GOING_TO_SUSPECT)
                {
                    bool criminalIsOnCar = CleoFunctions::IS_CHAR_IN_ANY_CAR(criminal->hPed);

                    if(criminalIsOnCar)
                    {
                        auto criminalCarHandle = CleoFunctions::ACTOR_USED_CAR(criminal->hPed);
                        float speed = CleoFunctions::CAR_SPEED(criminalCarHandle);

                        if(speed < 2.0f)
                        {
                            Log::Level(LOG_LEVEL::LOG_BOTH) << "suspect stopped, getting out of the car" << std::endl;

                            vehicle->MakePedsExitCar();
                            vehicle->actionStatus = ACTION_STATUS::WAITING_FOR_PEDS_TO_EXIT;
                            vehicle->followingCar = 0;
                        }
                    } else {
                        Log::Level(LOG_LEVEL::LOG_BOTH) << "suspect is on foot, getting out of the car" << std::endl;

                        vehicle->MakePedsExitCar();
                        vehicle->actionStatus = ACTION_STATUS::WAITING_FOR_PEDS_TO_EXIT;
                        vehicle->followingCar = 0;
                    }
                }
            }

            //if going to suspect and its position changes, drive to the new position
            if(vehicle->actionStatus == ACTION_STATUS::GOING_TO_SUSPECT)
            {
                auto distance = DistanceBetweenPoints(criminalPosition, vehicle->drivingTo);
                if(distance > 2.0f)
                {
                    bool criminalIsOnCar = CleoFunctions::IS_CHAR_IN_ANY_CAR(criminal->hPed);

                    if(criminalIsOnCar)
                    {
                        auto criminalCarHandle = CleoFunctions::ACTOR_USED_CAR(criminal->hPed);

                        if(vehicle->followingCar != criminalCarHandle)
                        {
                            vehicle->followingCar = criminalCarHandle;

                            if(vehicle->IsPoliceHelicopter())
                            {
                                Log::Level(LOG_LEVEL::LOG_BOTH) << "set heli " << vehicle->hVehicle << " follow criminal car " << criminalCarHandle << std::endl;

                                CleoFunctions::HELI_FOLLOW(vehicle->hVehicle, -1, criminalCarHandle, 20.0f);
                            } else {
                                Log::Level(LOG_LEVEL::LOG_BOTH) << "set car " << vehicle->hVehicle << " follow criminal car " << criminalCarHandle << std::endl;

                                CleoFunctions::SET_CAR_MAX_SPEED(vehicle->hVehicle, 30.0f);
                                CleoFunctions::CAR_FOLLOR_CAR(vehicle->hVehicle, criminalCarHandle, 8.0f);
                            }
                        }
                    } else {
                        Log::Level(LOG_LEVEL::LOG_BOTH) << "criminal position changed, going to the new position" << std::endl;

                        if(vehicle->IsPoliceHelicopter())
                        {
                            Log::Level(LOG_LEVEL::LOG_BOTH) << "set heli follow criminal" << std::endl;

                            CleoFunctions::HELI_FOLLOW(vehicle->hVehicle, criminal->hPed, -1, 20.0f);
                        } else {
                            Log::Level(LOG_LEVEL::LOG_BOTH) << "set car drive to criminal position" << std::endl;

                            CleoFunctions::CAR_DRIVE_TO(vehicle->hVehicle, criminalPosition.x, criminalPosition.y, criminalPosition.z);
                        }

                        
                    }
                    
                    vehicle->drivingTo = criminalPosition;
                }
            }
        } else {
            //no criminal
        }

        Log::Level(LOG_LEVEL::LOG_UPDATE) << "b1" << std::endl;

        //if vehicle is going to the suspect, and the suspect dies/despawn
        if(vehicle->actionStatus == ACTION_STATUS::GOING_TO_SUSPECT)
        {
            int criminalHandle = vehicle->goingToPed;
            if(criminalHandle > 0)
            {
                if(!Mod::IsActorAliveAndDefined(criminalHandle))
                {
                    vehicle->actionStatus = ACTION_STATUS::ACTION_NONE;
                    vehicle->goingToPed = 0;
                }
            }
        }
    }

    //

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "b2" << std::endl;

    for(auto ped : m_BackupPeds)
    {
        auto isInCar = CleoFunctions::IS_CHAR_IN_ANY_CAR(ped->hPed);
        auto pedPosition = Mod::GetPedPosition(ped->hPed);
        
        bool enteringCarOrExiting = false;
        auto vehicle = GetCarThatCopBelongs(ped);
        if(vehicle)
        {
            if(vehicle->actionStatus == ACTION_STATUS::WAITING_FOR_PEDS_TO_ENTER_CAR_AND_DO_NOTHING ||
                vehicle->actionStatus == ACTION_STATUS::WAITING_FOR_PEDS_TO_EXIT)
            {
                enteringCarOrExiting = true;
            }
        }

        //if ped is on foot, make it arrest the closest criminal
        if(!isInCar)
        {
            if(!enteringCarOrExiting)
            {
                if(ped->goingToPed == 0)
                {
                    auto criminal = Callouts::GetClosestCriminal(pedPosition);

                    if(criminal)
                    {
                        auto criminalPosition = Mod::GetPedPosition(criminal->hPed);

                        if(DistanceBetweenPoints(pedPosition, criminalPosition) < distanceEnterCar)
                        {
                            if(criminal->willShootAtCops)
                            {
                                Log::Level(LOG_LEVEL::LOG_BOTH) << "backup " << ped->hPed << " is now killing criminal" << std::endl;

                                CleoFunctions::KILL_ACTOR(ped->hPed, criminal->hPed);
                            } else {
                                Log::Level(LOG_LEVEL::LOG_BOTH) << "backup " << ped->hPed << " is now walking to criminal" << std::endl;

                                CleoFunctions::AS_ACTOR_RUN_TO_ACTOR(ped->hPed, criminal->hPed, 10000, 3.0f);

                                criminal->shouldHandsup = true;
                            }

                            ped->goingToPed = criminal->hPed;
                        } else {
                            Log::Level(LOG_LEVEL::LOG_BOTH) << "criminal is too far away" << std::endl;
                        }
                    }
                } else {
                    auto criminalIsAlive = Mod::IsActorAliveAndDefined(ped->goingToPed);

                    if(!criminalIsAlive)
                    {
                        ped->goingToPed = 0;

                        Log::Level(LOG_LEVEL::LOG_BOTH) << "backup killed its target" << std::endl;
                    }
                }
            }
        }
    }

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "b3" << std::endl;

    for(auto vehicle : m_BackupVehicles)
    {
        auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);
        auto criminal = Callouts::GetClosestCriminal(vehiclePosition);
        
        //if criminal is too far away from car, make peds enter vehicle
        if(criminal)
        {
            auto criminalPosition = Mod::GetPedPosition(criminal->hPed);

            if(DistanceBetweenPoints(vehiclePosition, criminalPosition) > distanceEnterCar)
            {
                if(vehicle->actionStatus == ACTION_STATUS::ACTION_NONE)
                {
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "criminal is too far away, entering car" << std::endl;

                    vehicle->actionStatus = ACTION_STATUS::WAITING_FOR_PEDS_TO_ENTER_CAR_AND_DO_NOTHING;
                    vehicle->MakePedsEnterVehicle();

                    auto owners = vehicle->GetOwners();
                    for(auto owner : owners)
                    {
                        if(Mod::IsActorAliveAndDefined(owner)) Peds::GetPedByHandle(owner)->goingToPed = 0;
                    }
                }
            }
        }

        //cops in passengers should drive by
        auto passengersHandle = vehicle->GetPassengers();
        for(auto passengerHandle : passengersHandle)
        {
            auto passenger = Peds::GetPedByHandle(passengerHandle);
            auto passengerPos = Mod::GetPedPosition(passengerHandle);
            
            auto closestCriminal = Callouts::GetClosestCriminal(passengerPos);

            if(closestCriminal)
            {
                if(closestCriminal->willShootAtCops)
                {
                    if(passenger->shootingAtPed != closestCriminal->hPed)
                    {
                        passenger->shootingAtPed = closestCriminal->hPed;

                        Log::Level(LOG_LEVEL::LOG_BOTH) << "Cop passenger " << passengerHandle << " is now shooting at criminal: " << closestCriminal->hPed << std::endl;

                        CleoFunctions::ACTOR_DRIVEBY(passengerHandle, closestCriminal->hPed, -1, 0, 0, 0, 100.0f, 4, true, 90);
                    }
                } else {
                    passenger->shootingAtPed = 0;
                }
            } else {
                passenger->shootingAtPed = 0;
            }
        }
    }

    for(auto vehicle : m_BackupVehicles)
    {
        auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);
        auto criminal = Callouts::GetClosestCriminal(vehiclePosition);

        if(!criminal)
        {
            if(vehicle->actionStatus == ACTION_STATUS::ACTION_NONE)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "No criminal in scene, leaving" << std::endl;

                vehicle->MakePedsEnterVehicleAndLeaveScene();
            }
        }
    }

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "b4" << std::endl;

    //remove peds 2
    //yes, it needs to be removed twice
    //(but i changed the code, idk if i need this anymore)
    //(yes, I still need it)
    //(maybe not anymore [yeah, i changed again])
    /*
    for(auto item : pedsToRemove)
    {
        auto it = std::find(m_BackupPeds.begin(), m_BackupPeds.end(), item);
        m_BackupPeds.erase(it);
    }
    pedsToRemove.clear();
    */
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

    PlayRequestBackupAudio(backupVehicle);

    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 150, 0, &x, &y, &z);

    float spawnX = 0, spawnY = 0, spawnZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(x, y, z, 2, &spawnX, &spawnY, &spawnZ);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "spawnX = " << spawnX << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "spawnY = " << spawnY << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "spawnZ = " << spawnZ << std::endl;

    auto car = CleoFunctions::CREATE_CAR_AT(backupVehicle->vehicleModelId, spawnX, spawnY, spawnZ);
    Log::Level(LOG_LEVEL::LOG_BOTH) << "car = " << car << std::endl;

    auto vehicle = Vehicles::TryCreateVehicle(car);
    //vehicle->AddBlip(2);

    /*
    if(m_BackupType == BACKUP_TYPE::BACKUP_CHASE)
    {
        vehicle->actionStatus = ACTION_STATUS::CHASE_CHASING;
    } else if (m_BackupType == BACKUP_TYPE::BACKUP_CALLOUT)
    {
        vehicle->actionStatus = ACTION_STATUS::CALLOUT_GOING_TO_CALLOUT;
    }
    */

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
    m_BackupPeds.push_back(pedDriver);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "driver = " << driver << std::endl;

    CleoFunctions::GIVE_ACTOR_WEAPON(driver, backupVehicle->weaponId, 10000);

    for(int i = 0; i < backupVehicle->numPeds - 1; i++)
    {
        int passenger = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(car, type, backupVehicle->pedModelId, i);
        auto pedPassenger = Peds::TryCreatePed(passenger);
        pedPassenger->AddBlip(2);
        m_BackupPeds.push_back(pedPassenger);

        CleoFunctions::GIVE_ACTOR_WEAPON(passenger, backupVehicle->weaponId, 10000);
    }

    vehicle->SetDriverAndPassengersOwners();

    CleoFunctions::ENABLE_CAR_SIREN(car, true);
    CleoFunctions::SET_CAR_MAX_SPEED(car, 50.0f);
    //0423: set_car 6@ improved_handling_to 1.5
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(car, 2);
    
    /*
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
    */

    Log::Level(LOG_LEVEL::LOG_BOTH) << "end call backup" << std::endl;
}

void Backup::CallBackupHeli()
{
    PlayRequestBackupAudio(&m_DataBackupVehicles[6]); //6 = polmav

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
    heliVehicle->SetDriverAndPassengersOwners();

    /*
    if(m_BackupType == BACKUP_TYPE::BACKUP_CHASE)
    {
        heliVehicle->actionStatus = ACTION_STATUS::CHASE_CHASING;
    } else if (m_BackupType == BACKUP_TYPE::BACKUP_CALLOUT)
    {
        heliVehicle->actionStatus = ACTION_STATUS::CALLOUT_ACTING_ON_CALLOUT;
    }*/

    m_BackupVehicles.push_back(heliVehicle);

    /*
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
    */

    CleoFunctions::WAIT(3000, []() {
        CleoFunctions::SHOW_TEXT_BOX("MPFX86");
    });
}

void Backup::PlayRequestBackupAudio(BackupVehicle* backupVehicle)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Backup: PlayRequestBackupAudio" << std::endl;

    m_IncomingBackupSoundName = backupVehicle->soundIncoming;

    if(m_RequestBackupAudio != NULL)
    {
        m_RequestBackupAudio->Stop();
        m_RequestBackupAudio->Destroy();
        m_RequestBackupAudio = NULL;
    }

    SoundSystem::PlayHTAudio();
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "playing audio: " << backupVehicle->soundRequest << std::endl;
    m_RequestBackupAudio = SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation(backupVehicle->soundRequest, false);

    m_WaitingToRespondDispatch = true;
}

int Backup::FindClosestCop(CVector position, float radius, bool includePlayer)
{
    std::vector<int> cops;
    for(auto ped : m_BackupPeds)
    {
        auto copPosition = Mod::GetPedPosition(ped->hPed);
        auto distance = DistanceBetweenPoints(position, copPosition);

        if(distance > radius) continue;

        cops.push_back(ped->hPed);
    }

    if(includePlayer)
    {
        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        cops.push_back(playerActor);
    }

    if(cops.size() == 0) return 0;

    int closestCopIndex = 0;
    double closestDistance = INFINITY;

    for(size_t i = 0; i < cops.size(); i++)
    {
        auto copHandle = cops[0];
        
        auto copPosition = Mod::GetPedPosition(copHandle);

        auto distance = DistanceBetweenPoints(position, copPosition);

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closestCopIndex = i;
        }
    }

    return cops[closestCopIndex];
}

Vehicle* Backup::GetCarThatCopBelongs(Ped* cop)
{
    auto copHandle = cop->hPed;

    for(auto vehicle : m_BackupVehicles)
    {
        if(vehicle->hDriverOwner == copHandle) return vehicle;
        for(auto passengerHandle : vehicle->hPassengersOwner)
        {
            if(passengerHandle == copHandle) return vehicle;
        }
    }

    return NULL;
}