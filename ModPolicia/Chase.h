#pragma once

#include "pch.h"

#include "Ped.h"

class Chase {
public:
    static Ped* m_ChasingPed;

    static void Update(int dt);
    static void UpdateChase(int dt);

    static void MakeCarStartRunning(Vehicle* vehicle, Ped* ped);
    static void EndChase();
};