#include "Vehicles.h"

#include "SimpleGTA.h"

#include "Log.h"

extern uintptr_t* pVehiclePool;
extern int (*GetVehicleRef)(int);

std::map<int, Vehicle*> Vehicles::m_Vehicles;

void Vehicles::Update(int dt)
{
    TryFindNewVehicles();
}

void Vehicles::TryFindNewVehicles()
{
}

bool Vehicles::HasVehicleHandle(int hVehicle)
{
    return m_Vehicles.find(hVehicle) != m_Vehicles.end();
}

Vehicle* Vehicles::TryCreateVehicle(int hVehicle)
{
	if (HasVehicleHandle(hVehicle))
    {
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