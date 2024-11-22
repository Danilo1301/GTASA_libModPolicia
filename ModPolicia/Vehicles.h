#pragma once

#include "pch.h"

#include "Vehicle.h"

class Vehicles {
public:
    static std::map<int, Vehicle*> m_Vehicles;

    static void Update(int dt);
    static void Draw();

    static void TryFindNewVehicles();
    static void TryDeleteVehicles();

    static bool HasVehicleHandle(int hVehicle);
    static Vehicle* TryCreateVehicle(int hVehicle);
    static void RemoveVehicle(Vehicle* vehicle);
    static Vehicle* GetVehicleByHandle(int hVehicle);

    static std::vector<Vehicle*> GetAllCarsInSphere(CVector position, float radius);
    static int GetRandomCarInSphere(CVector position, float radius);
    static int GetClosestCar(CVector position, float radius);
    static std::vector<Vehicle*> GetDefinedVehicles();
};