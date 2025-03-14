#pragma once

#include "pch.h"

class BikePickpocket {
public:
    static float m_TimeBetweenPickpockets;

    static void Update(int dt);
    static void UpdateSpawnPickpocket(int dt);
    static void SpawnPickpocketCloseToPlayer();
    static void SpawnPickpocket(CVector position);
    static void FinishPickpocket();
};