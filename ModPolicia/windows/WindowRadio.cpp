#include "WindowRadio.h"

#include "CleoFunctions.h"
#include "Mod.h"
#include "Ambulance.h"

Window* WindowRadio::m_Window = NULL;

void WindowRadio::Create()
{
	if (m_Window) return;

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto button_ambulance = window->AddButton(117, 0, 0);
    button_ambulance->onClick = []()
    {
        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        auto playerPosition = Mod::GetPedPosition(playerActor);

        Ambulance::CallAmbulance(playerPosition);

        Remove();
    };

    auto button_IML = window->AddButton(118, 0, 0);
    button_IML->onClick = []()
    {
        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        auto playerPosition = Mod::GetPedPosition(playerActor);

        Ambulance::CallIML(playerPosition);
        
        Remove();
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowRadio::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}