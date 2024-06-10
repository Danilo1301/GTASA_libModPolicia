#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"

struct Barrier {
    int objectId = 0;
    CVector objectPosition = CVector(0, 0, 0);

    int ped1 = 0;
    int ped2 = 0;
    int object = 0;
    int car = 0;
    int marker = 0;
};

class Chase {
public:
    static Ped* m_ChasingPed;
    static std::vector<Barrier*> m_Barriers;

    static void Update(int dt);
    static void UpdateChase(int dt);
    static void UpdateBarriers(int dt);

    static void MakeCarStartRunning(Vehicle* vehicle, Ped* ped);
    static void EndChase();

    static Barrier* AddBarrier(CVector position, int objectId, int carModelId, int pedModelId);
    static void AddRoadBlocks(CVector position);
    static void AddSpikestrips(CVector position);

    static void DeflateCarTires(int hVehicle);
};