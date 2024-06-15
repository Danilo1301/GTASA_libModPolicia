#pragma once

#include "pch.h"

#include "TrunkModelData.h"
#include "Ped.h"

class TrunkData {
public:
    int modelId;
    int hVehicle;
    std::vector<int> pedsInTrunk;
    int checkpoint = 0;

    TrunkData(int modelId, int hVehicle);

    bool HasModelData();
    TrunkModelData* GetModelData();

    void PutPedAtTrunk(int hPed);
    void RemovePedFromTrunk(int hPed);
    void RemoveAllPedsFromTrunk();
};