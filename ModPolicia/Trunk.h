#pragma once

#include "pch.h"

#include "TrunkModelData.h"
#include "Ped.h"
#include "Vehicle.h"

class Trunk {
public:
    static std::map<int, TrunkModelData> m_TrunkModels;
    static Vehicle* m_PreviewVehicle;

    static void Update(int dt);

    static void CreatePreviewPeds(Vehicle* vehicle);
    static void UpdatePreviewPeds();
    static void DestroyPreviewPeds();
};