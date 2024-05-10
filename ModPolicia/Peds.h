#pragma once

#include "pch.h"

#include "Ped.h"

class Peds {
public:
    static std::map<int, Ped*> m_Peds;

    static void Update(int dt);

    static void TryFindNewPeds();

    static bool HasPedHandle(int hPed);
    static Ped* TryCreatePed(int hPed);
    static Ped* GetPedByHandle(int hPed);
};