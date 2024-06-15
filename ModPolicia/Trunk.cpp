#include "Trunk.h"

#include "Vehicles.h"
#include "Scorch.h"
#include "Mod.h"
#include "CleoFunctions.h"
#include "Peds.h"
#include "Log.h"
#include "Callouts.h"

std::map<int, TrunkModelData> Trunk::m_TrunkModels = {
    {596, {
        {CVector(-0.50f, -1.80f, 0.90f), CVector(0.50f, -1.80f, 0.90f)}, {90.0f, -90.0f}
    }},
    {597, {
        {CVector(-0.50f, -2.00f, 0.60f), CVector(0.50f, -2.00f, 0.60f)}, {90.0f, -90.0f}
    }},
    {598, {
        {CVector(-0.50f, -1.80f, 0.90f), CVector(0.50f, -1.80f, 0.90f)}, {90.0f, -90.0f}
    }},
    {599, {
        {CVector(-0.50f, -1.80f, 0.90f), CVector(0.50f, -1.80f, 0.90f)}, {90.0f, -90.0f}
    }},
    {528, {
        {CVector(-0.50f, -1.80f, 0.90f), CVector(0.50f, -1.80f, 0.90f)}, {90.0f, -90.0f}
    }},
    {490, {
        {CVector(-0.50f, -1.80f, 0.90f), CVector(0.50f, -1.80f, 0.90f)}, {90.0f, -90.0f}
    }},
    {601, {
        {CVector(-0.50f, -1.80f, 0.90f), CVector(0.50f, -1.80f, 0.90f)}, {90.0f, -90.0f}
    }},
};
Vehicle* Trunk::m_PreviewVehicle = NULL;

void Trunk::Update(int dt)
{
    auto vehicles = Vehicles::GetDefinedVehicles();

    for(auto vehicle : vehicles)
    {
        if(!vehicle->trunk->HasModelData()) continue;

        auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);
        auto playerActor = Mod::GetPlayerActor();
        auto playerPosition = Mod::GetPedPosition(playerActor);
        
        auto distanceFromVehicle = DistanceBetweenPoints(playerPosition, vehiclePosition);

        if(distanceFromVehicle < 20.0f && Scorch::m_CarryingPed)
        {
            auto spherePosition = Mod::GetCarPositionWithOffset(vehicle->hVehicle, CVector(0, -4.0f, 0));

            if(vehicle->trunk->checkpoint == 0)
            {
                vehicle->trunk->checkpoint = CleoFunctions::CREATE_SPHERE(spherePosition.x, spherePosition.y, spherePosition.z, 2.0f);
            }

            auto distanceFromSphere = DistanceBetweenPoints(playerPosition, spherePosition);

            if(distanceFromSphere < 2.0f)
            {
                vehicle->trunk->PutPedAtTrunk(Scorch::m_CarryingPed->hPed);

                Callouts::RemovePedFromCriminalList(Scorch::m_CarryingPed);

                Scorch::m_CarryingPed = NULL;

                CleoFunctions::DESTROY_OBJECT(Scorch::m_CarryObject);
                Scorch::m_CarryObject = 0;
            }
        }
        
        if(distanceFromVehicle > 20.0f || !Scorch::m_CarryingPed)
        {
            if(vehicle->trunk->checkpoint != 0)
            {
                CleoFunctions::DESTROY_SPHERE(vehicle->trunk->checkpoint);
                vehicle->trunk->checkpoint = 0;
            }
        }
    }
}

void Trunk::CreatePreviewPeds(Vehicle* vehicle)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "CreatePreviewPeds" << std::endl;

    m_PreviewVehicle = vehicle;

    auto playerActor = Mod::GetPlayerActor();
    auto position = Mod::GetPedPositionWithOffset(playerActor, CVector(0, 2, 0));

    auto ped1Handle = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, 19, position.x, position.y, position.z);
    
    auto ped2Handle = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, 193, position.x, position.y, position.z);

    vehicle->trunk->PutPedAtTrunk(ped1Handle);
    vehicle->trunk->PutPedAtTrunk(ped2Handle);
}

void Trunk::UpdatePreviewPeds()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "UpdatePreviewPeds" << std::endl;

    auto vehicle = m_PreviewVehicle;

    auto ped1Handle = m_PreviewVehicle->trunk->pedsInTrunk[0];
    auto ped2Handle = m_PreviewVehicle->trunk->pedsInTrunk[1];
    
    vehicle->trunk->RemoveAllPedsFromTrunk();

    vehicle->trunk->PutPedAtTrunk(ped1Handle);
    vehicle->trunk->PutPedAtTrunk(ped2Handle);
}

void Trunk::DestroyPreviewPeds()
{
    auto vehicle = m_PreviewVehicle;

    auto ped1Handle = m_PreviewVehicle->trunk->pedsInTrunk[0];
    auto ped2Handle = m_PreviewVehicle->trunk->pedsInTrunk[1];

    vehicle->trunk->RemoveAllPedsFromTrunk();

    CleoFunctions::DESTROY_ACTOR(ped1Handle);
    CleoFunctions::DESTROY_ACTOR(ped2Handle);

    m_PreviewVehicle = NULL;
}