#pragma once

#include "pch.h"

#include "Pickup.h"

class PoliceDepartment {
public:
    static Pickup* m_PickupMenu;
    static Pickup* m_PickupEquipment;

    static void Init();
    static void Update(int dt);
};