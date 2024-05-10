#pragma once

#include "pch.h"

#include "Ped.h"

class Pullover {
public:
    static Ped* m_PullingPed;

    static void Update(int dt);
    static void TryPullOverPedOrCar();
    static void FriskPed();
    static void FreePed();
};