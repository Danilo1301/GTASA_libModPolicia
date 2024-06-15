#include "TrunkData.h"

#include "Trunk.h"
#include "Log.h"
#include "CleoFunctions.h"

TrunkData::TrunkData(int modelId, int hVehicle)
{
    this->modelId = modelId;
    this->hVehicle = hVehicle;
}

bool TrunkData::HasModelData()
{
    return Trunk::m_TrunkModels.find(modelId) != Trunk::m_TrunkModels.end();
}

TrunkModelData* TrunkData::GetModelData()
{
    return &Trunk::m_TrunkModels[modelId];
}

void TrunkData::PutPedAtTrunk(int hPed)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "PutPedAtTrunk" << std::endl;

    CleoFunctions::REMOVE_ACTOR_FROM_TURRET_MODE(hPed);

    auto trunkIndex = 0;
    if(pedsInTrunk.size() > 0) trunkIndex = 1;

    auto trunkModelData = GetModelData();
    auto trunkPosition = trunkModelData->trunkPositions[trunkIndex];
    //auto angle = trunkModelData->trunkAngles[trunkIndex];

    //CleoFunctions::SET_ACTOR_ANGLE_TO(hPed, angle);

    CleoFunctions::PUT_ACTOR_INTO_TURRET_ON_CAR(hPed, hVehicle, trunkPosition.x, trunkPosition.y, trunkPosition.z, 0, 0.0f, 0);

    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(hPed, "cower", "PED", 4.0f, 0, 0, 0, 1, -1);

    pedsInTrunk.push_back(hPed);
}

void TrunkData::RemovePedFromTrunk(int hPed)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "RemovePedFromTrunk" << std::endl;

    CleoFunctions::REMOVE_ACTOR_FROM_TURRET_MODE(hPed);

    auto it = std::find(pedsInTrunk.begin(), pedsInTrunk.end(), hPed);
    pedsInTrunk.erase(it);
}

void TrunkData::RemoveAllPedsFromTrunk()
{
    while(pedsInTrunk.size() > 0)
    {
        RemovePedFromTrunk(pedsInTrunk[0]);
    }
}
