#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"

class Chase {
public:
    static Ped* m_ChasingPed;
    static std::vector<Vehicle*> m_BackupVehicles;

    static void Update(int dt);
    static void UpdateChase(int dt);
    static void UpdateBackup(int dt);

    static void CallBackup(int vehicleModelId, int pedModelId);

    static void MakeCarStartRunning(Vehicle* vehicle, Ped* ped);
    static void EndChase();
};