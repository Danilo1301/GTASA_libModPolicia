#include "Vehicles.h"

#include "SimpleGTA.h"

#include "Log.h"
#include "CleoFunctions.h"
#include "Mod.h"

extern uintptr_t* pVehiclePool;
extern int (*GetVehicleRef)(int);

std::map<int, Vehicle*> Vehicles::m_Vehicles;

void Vehicles::Update(int dt)
{
    TryFindNewVehicles();

    for(auto pair : m_Vehicles)
    {
        auto vehicle = pair.second;

        vehicle->Update(dt);
    }
}

void Vehicles::TryFindNewVehicles()
{   
    //GET_RANDOM_CAR_IN_SPHERE_NO_SAVE_RECURSIVE
    //https://github.com/AndroidModLoader/GTA_CLEOMod/blob/RUSJJ_CLEO_MOD/cleo4opcodes.cpp

    auto objects = *(GTAVehicleSA**)(*pVehiclePool + 0);
    tByteFlag* flags = *(tByteFlag**)(*pVehiclePool + 4);
    int size = *(int*)(*pVehiclePool + 8);

    for (int i = 0; i < size; ++i)
    {
        if (flags[i].bEmpty) continue;
        auto& ent = objects[i];
        
        //IS_CAR_SIREN_ON
        //https://github.com/AndroidModLoader/GTA_CLEOMod/blob/RUSJJ_CLEO_MOD/cleo4opcodes.cpp
        // 
        //bool sirenOn = *(uint8_t*)((uintptr_t)vehicle + 0x42D + 4) >> 7;
        bool sirenOn = ent.UInt8At(0x42D + 4) >> 7;

        void* driver = (void*)ent.UIntAt(0x460 + 4);

        int ref = GetVehicleRef(ent.AsInt());

        if(!HasVehicleHandle(ref))
        {
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "Found " << ent.AsInt() << " ref " << ref << " siren " << (sirenOn ? "ON" : "OFF") << std::endl;
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "driver " << driver << std::endl;

            auto vehicle = TryCreateVehicle(ref);
        }
    }
}

bool Vehicles::HasVehicleHandle(int hVehicle)
{
    return m_Vehicles.find(hVehicle) != m_Vehicles.end();
}

Vehicle* Vehicles::TryCreateVehicle(int hVehicle)
{   
    if(hVehicle == 0)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Bro, i think your code has some issues (ERROR) (Vehicles)" << std::endl;
        return NULL;
    }

	if (HasVehicleHandle(hVehicle))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle already added: " << hVehicle << std::endl;
        return GetVehicleByHandle(hVehicle);
    }

    auto vehicle = new Vehicle(hVehicle);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Add vehicle " << hVehicle << ", ref: " << vehicle->pVehicle << ", model: " << vehicle->modelId << " (" << std::to_string(m_Vehicles.size() + 1) << " total)" << std::endl;

	m_Vehicles[hVehicle] = vehicle;

    return vehicle;
}

Vehicle* Vehicles::GetVehicleByHandle(int hVehicle)
{
	if (!HasVehicleHandle(hVehicle)) return NULL;
	return m_Vehicles.at(hVehicle);
}

std::vector<Vehicle*> Vehicles::GetAllCarsInSphere(CVector position, float radius)
{
    std::vector<Vehicle*> vehicles;

    auto allVehicles = GetDefinedVehicles();

    for(auto vehicle : allVehicles)
    {
        auto vehiclePos = Mod::GetCarPosition(vehicle->hVehicle);
        auto distance = DistanceBetweenPoints(position, vehiclePos);

        if(distance <= radius)
        {
            vehicles.push_back(vehicle);
        }
    }

    return vehicles;
}

int Vehicles::GetRandomCarInSphere(CVector position, float radius)
{
    std::vector<Vehicle*> vehicles = GetAllCarsInSphere(position, radius);

    if(vehicles.size() == 0) return 0;

    return vehicles[0]->hVehicle;
}

int Vehicles::GetClosestCar(CVector position, float radius)
{
    std::vector<Vehicle*> vehicles = GetAllCarsInSphere(position, radius);

    Vehicle* closestCar = NULL;
    double closestDistance = INFINITY;

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
    CleoFunctions::GET_CHAR_COORDINATES(playerActor, &playerX, &playerY, &playerZ);

    for(size_t i = 0; i < vehicles.size(); i++)
    {
        auto vehicle = vehicles[i];
        auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);
        
        auto distance = DistanceBetweenPoints(vehiclePosition, CVector(playerX, playerY, playerZ));

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closestCar = vehicle;
        }
    }

    if(!closestCar) return -1;

    return closestCar->hVehicle;
}

std::vector<Vehicle*> Vehicles::GetDefinedVehicles()
{
    std::vector<Vehicle*> vehicles;

    for(auto pair : m_Vehicles)
    {
        auto vehicle = pair.second;

        if(!CleoFunctions::CAR_DEFINED(vehicle->hVehicle)) continue;

        vehicles.push_back(vehicle);
    }

    return vehicles;
}