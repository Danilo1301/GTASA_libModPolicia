#include "Pullover.h"

#include "Widgets.h"
#include "Log.h"
#include "Peds.h"
#include "CleoFunctions.h"

#include "windows/WindowTest.h"
#include "windows/WindowFrisk.h"

Ped* Pullover::m_PullingPed = NULL;

void Pullover::Update(int dt)
{
    if(Widgets::IsWidgetJustPressed(7))
    {
        Log::file << "[Mod] Widget 7 has just been pressed" << std::endl;

        TryPullOverPedOrCar();
    }
}

void Pullover::TryPullOverPedOrCar()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0.0f, y = 0.0f, z = 0.0f;
    CleoFunctions::GET_CHAR_COORDINATES(playerActor, &x, &y, &z);

    int randomChar = CleoFunctions::GET_RANDOM_CHAR_IN_SPHERE(x, y, z, 5.0f, true, true, true);
    
    if(randomChar == -1)
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX32", 0, 0, 0, 3000, 1); //nao encontrado
        return;
    }

    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "CopTraf_Stop", "POLICE", 4.0f, 0, 0, 0, 0, -1);

    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX31", 0, 0, 0, 3000, 1); //parado!
    
    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(randomChar, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1);

    m_PullingPed = Peds::TryCreatePed(randomChar);

    CleoFunctions::WAIT(2000, [](){
        WindowTest::Create();
    });
}

void Pullover::FriskPed()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "hndshkfa_swt", "gangs", 2.0f, 0, 0, 0, 0, -1);

    m_PullingPed->UpdateInventory();

    CleoFunctions::WAIT(3500, []() {
        WindowFrisk::Create();
    });
}

void Pullover::FreePed()
{
    //CleoFunctions::SET_PLAYER_CAN_MOVE(m_PullingPed->hPed, true);
    CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(m_PullingPed->hPed);

    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX55", 0, 0, 0, 3000, 1); //liberado
}