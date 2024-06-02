#include "WindowRadio.h"

#include "CleoFunctions.h"
#include "Mod.h"
#include "Ambulance.h"
#include "Callouts.h"
#include "SoundSystem.h"

Window* WindowRadio::m_Window = NULL;

void WindowRadio::Create()
{
	if (m_Window) return;

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    if(Callouts::m_AproachingCallout)
    {
        auto button_abortCallout = window->AddButton(119, 0, 0);
        button_abortCallout->onClick = []()
        {
            Callouts::AbortCallout();
            Remove();
        };
    }

    auto button_ambulance = window->AddButton(117, 0, 0);
    button_ambulance->onClick = []()
    {
        SoundSystem::PlayHTAudio();
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX120", 0, 0, 0, 3000, 1); //apoio ambulancia

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        auto playerPosition = Mod::GetPedPosition(playerActor);

        Ambulance::CallAmbulance(playerPosition);

        Remove();
    };

    auto button_IML = window->AddButton(118, 0, 0);
    button_IML->onClick = []()
    {
        SoundSystem::PlayHTAudio();
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX121", 0, 0, 0, 3000, 1); //apoio IML

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