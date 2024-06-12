#pragma once

#include "pch.h"

class Pickup {
public:
    CVector position = CVector(0, 0, 0);
    int pickupModelId;

    int sphere = 0;
    int pickup = 0;

    bool isPlayerInPickup = false;
    bool playerJustPickedPickup = false;

    Pickup(CVector position, int pickupModelId);

    void Update(int dt);
};