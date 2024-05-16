#include "Vehicles.h"

#include "SimpleGTA.h"

#include "Log.h"

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
            auto vehicle = TryCreateVehicle(ref);

            Log::file << "Found " << ent.AsInt() << " ref " << ref << " siren " << (sirenOn ? "ON" : "OFF") << std::endl;
            Log::file << "driver " << driver << std::endl;
        }
    }
}

bool Vehicles::HasVehicleHandle(int hVehicle)
{
    return m_Vehicles.find(hVehicle) != m_Vehicles.end();
}

Vehicle* Vehicles::TryCreateVehicle(int hVehicle)
{
	if (HasVehicleHandle(hVehicle))
    {
        Log::file << "Vehicles already added: " << hVehicle << std::endl;
        return GetVehicleByHandle(hVehicle);
    }

	Log::file << "Vehicles: Add vehicle " << hVehicle << "(" << std::to_string(m_Vehicles.size() + 1) << " total)" << std::endl;

    auto vehicle = new Vehicle(hVehicle);

	m_Vehicles[hVehicle] = vehicle;

    return vehicle;
}

Vehicle* Vehicles::GetVehicleByHandle(int hVehicle)
{
	if (!HasVehicleHandle(hVehicle)) return NULL;
	return m_Vehicles.at(hVehicle);
}