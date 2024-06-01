#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"
#include "Locations.h"

struct ScorchPedData {
    Ped* ped = NULL;
    Location toDp = {{0, 0, 0}, 0, CITY::LOS_SANTOS};

    bool followingPlayer = false;

    int sphere = 0;
    int blip = 0;

    Vehicle* vehicle = NULL;
};

class Scorch {
public:
    static std::vector<ScorchPedData*> m_ScorchingPeds;

    static std::vector<Vehicle*> m_TowTrucks;

    static void Update(int dt);
    static void UpdateTowTrucks(int dt);
    static void UpdateScorchingPeds(int dt);

    static void StartScorchingPed(Ped* ped);
    static void CallVehicleToScorchPed(Ped* ped);
    static void TeleportPedToPrision(Ped* ped);

    static void DestroyScorchData(ScorchPedData* data);

    static bool IsPedBeeingScorched(int hPed);

    static void CallTowTruckToVehicle(Vehicle* vehicle);
};