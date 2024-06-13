#include "Pickup.h"

#include "CleoFunctions.h"
#include "Mod.h"

Pickup::Pickup(CVector position, int pickupModelId)
{
    this->position = position;
    this->pickupModelId = pickupModelId;
}

void Pickup::Update(int dt)
{
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
    auto playerPosition = Mod::GetPedPosition(playerActor);
    auto distanceFromPlayer = DistanceBetweenPoints(playerPosition, position);

    if(distanceFromPlayer < 100.0f)
    {
        if(sphere == 0)
        {
            sphere = CleoFunctions::CREATE_SPHERE(position.x, position.y, position.z, 1.5f);
        }
        
        if(pickup == 0)
        {
            pickup = CleoFunctions::CREATE_PICKUP(pickupModelId, 6, position.x, position.y, position.z);
        }
    } else {
        if(sphere != 0)
        {
            CleoFunctions::DESTROY_SPHERE(sphere);
            sphere = 0;
        }

        if(pickup != 0)
        {
            CleoFunctions::DESTROY_PICKUP(pickup);
            pickup = 0;
        }
    }

    playerJustPickedPickup = false;
    
    if(distanceFromPlayer < 1.5f)
    {
        if(!isPlayerInPickup)
        {
            isPlayerInPickup = true;
            playerJustPickedPickup = true;
        }
    } else {
        isPlayerInPickup = false;
    }
}