#pragma once

#include "pch.h"

#include "Pickup.h"

class Base {
public:
    std::string name = "Base";

    Pickup* m_PickupDuty;
    Pickup* m_PickupEquipment;
    Pickup* m_PickupPartner;

    Base();

    CVector spawnVehiclePosition = CVector(0, 0, 0);
    float spawnVehicleAngle = 0.0f;

    CVector scorchSuspectPosition = CVector(0, 0, 0);
};