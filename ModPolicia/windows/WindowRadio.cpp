#include "WindowRadio.h"

#include "CleoFunctions.h"
#include "Mod.h"
#include "Ambulance.h"
#include "Callouts.h"
#include "SoundSystem.h"
#include "Chase.h"
#include "Pullover.h"
#include "ModConfig.h"
#include "Peds.h"
#include "Log.h"
#include "Vehicles.h"

Window* WindowRadio::m_Window = NULL;

#include "isautils.h"
extern ISAUtils* sautils;
int scmHandle1, scmHandle2;
float scmHandleFloat1, scmHandleFloat2, scmHandleFloat3;
static DEFOPCODE(01F5, GET_PLAYER_ACTOR, iv); //01F5: $PLAYER_ACTOR = get_player_actor $PLAYER_CHAR 
static DEFOPCODE(00A0, GET_CHAR_COORDINATES, ivvv);
static DEFOPCODE(036D, SHOW_TEXT_2NUMBERS_STYLED, siiii);
static DEFOPCODE(0256, IS_PLAYER_PLAYING, i); //0256:  player $PLAYER_CHAR defined 

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
            Remove();
            Callouts::AbortCallout();
        };
    }

    auto button_ambulance = window->AddButton(117, 0, 0);
    button_ambulance->onClick = []()
    {
        Remove();

        SoundSystem::PlayHTAudio();
        SoundSystem::PlayStreamFromAudiosFolder("voices/REQUEST_AMBULANCE.wav", false);
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX120", 0, 0, 0, 3000, 1); //apoio ambulancia

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        auto playerPosition = Mod::GetPedPosition(playerActor);

        Ambulance::CallAmbulance(playerPosition);
    };

    auto button_IML = window->AddButton(118, 0, 0);
    button_IML->onClick = []()
    {
        Remove();

        SoundSystem::PlayHTAudio();
        SoundSystem::PlayStreamFromAudiosFolder("voices/REQUEST_IML.wav", false);
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX121", 0, 0, 0, 3000, 1); //apoio IML

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        auto playerPosition = Mod::GetPedPosition(playerActor);

        Ambulance::CallIML(playerPosition);
    };

    if(ModConfig::CreateTestOptionsInRadioMenu)
    {
        CreateTestOptions();
    }

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

void WindowRadio::CreateTestOptions()
{
    auto window = m_Window;
    
    auto button_pedPullover = window->AddButton(127, 0, 0);
    button_pedPullover->onClick = []()
    {
        Remove();
        Pullover::CreateTestPullOverPed();
    };

    auto button_carPullover = window->AddButton(128, 0, 0);
    button_carPullover->onClick = []()
    {
        Remove();
        Pullover::CreateTestPullOverVehicle();
    };

    auto button_chaseEnd = window->AddButton(129, 0, 0);
    button_chaseEnd->onClick = []()
    {
        Remove();
        if(Chase::m_ChasingPed)
        {
            auto vehicle = Vehicles::GetVehicleByHandle(Chase::m_ChasingPed->hVehicleOwned);
            //Chase::EndChase();
            CleoFunctions::CLEAR_ACTOR_TASK(Chase::m_ChasingPed->hPed);
            CleoFunctions::EXIT_CAR_AS_ACTOR(Chase::m_ChasingPed->hPed);
            //vehicle->MakePedsExitCar();
            //Pullover::m_PullingVehicle = vehicle;
            //Pullover::m_PullingPed = Peds::GetPedByHandle(vehicle->GetDriver());
            //Pullover::AskPedsToLeaveCar(vehicle);
        }
    };

    auto button_test7 = window->AddButton(23, 7, 0);
    button_test7->onClick = []()
    {
        Remove();

        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        auto position = Mod::GetPedPositionWithOffset(playerActor, CVector(0, 2, 0));

        auto pedHandle = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, 19, position.x, position.y, position.z);
        auto ped = Peds::TryCreatePed(pedHandle);
        ped->AddBlip();

        Callouts::m_Criminals.push_back(ped);
    };

    auto button_test8 = window->AddButton(23, 8, 0);
    button_test8->onClick = []()
    {
        Remove();

        auto chasingPed = Chase::m_ChasingPed;

        if(chasingPed) Chase::DeflateCarTires(chasingPed->hVehicleOwned);
    };
    
    auto button_test6 = window->AddButton(23, 6, 0);
    button_test6->onClick = []()
    {
        Remove();

        auto peds = Peds::GetDeadPeds();

        for(auto ped : peds)
        {
            auto pedPosition = Mod::GetPedPosition(ped->hPed);

            auto sphere = CleoFunctions::CREATE_SPHERE(pedPosition.x, pedPosition.y, pedPosition.z, 3.0);
        }

        Log::Level(LOG_LEVEL::LOG_BOTH) << peds.size() << " dead peds found" << std::endl;
    };

    auto button_test1 = window->AddButton(23, 1, 0);
    button_test1->onClick = []()
    {
        Remove();

        /*
        Log::file << "scm_GET_PLAYER_ACTOR" << std::endl;
        scmHandle1 = 0;
        sautils->ScriptCommand(&scm_GET_PLAYER_ACTOR, 0, &scmHandle1);
        Log::file << "scmHandle1 = " << scmHandle1 << std::endl;
        int playerActor = scmHandle1;

        Log::file << "scm_GET_CHAR_COORDINATES" << std::endl;
        scmHandleFloat1 = 0.0;
        scmHandleFloat2 = 0.0;
        scmHandleFloat3 = 0.0;
        sautils->ScriptCommand(&scm_GET_CHAR_COORDINATES, playerActor, &scmHandleFloat1, &scmHandleFloat2, &scmHandleFloat3);
        float x = scmHandleFloat1, y = scmHandleFloat2, z = scmHandleFloat3;
        Log::file << "x = " << x << std::endl;
        Log::file << "y = " << y << std::endl;
        Log::file << "z = " << z << std::endl;

        int intX = (int)x;
        int intY = (int)y;
        Log::file << "scm_SHOW_TEXT_2NUMBERS_STYLED" << std::endl;
        sautils->ScriptCommand(&scm_SHOW_TEXT_2NUMBERS_STYLED, "MPFX2", intX, intY, 5000, 5);
        */

        //test 2
        int playerActor = 0;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "scm_GET_PLAYER_ACTOR" << std::endl;
        sautils->ScriptCommand(&scm_GET_PLAYER_ACTOR, 0, &playerActor);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "playerActor = " << playerActor << std::endl;

        float x = 0.0f, y = 0.0f, z = 0.0f;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "scm_GET_CHAR_COORDINATES" << std::endl;
        sautils->ScriptCommand(&scm_GET_CHAR_COORDINATES, playerActor, &x, &y, &z);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "x = " << x << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "y = " << y << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "z = " << z << std::endl;

        int intX = (int)x;
        int intY = (int)y;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "scm_SHOW_TEXT_2NUMBERS_STYLED" << std::endl;
        sautils->ScriptCommand(&scm_SHOW_TEXT_2NUMBERS_STYLED, "MPFX2", intX, intY, 5000, 5);

        Log::Level(LOG_LEVEL::LOG_BOTH) << "scm_IS_PLAYER_PLAYING" << std::endl;
        bool result = sautils->ScriptCommand(&scm_IS_PLAYER_PLAYING, 0);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "result = " << (result ? "1" : "0") << std::endl;

        if(result)
        {
            sautils->ScriptCommand(&scm_SHOW_TEXT_2NUMBERS_STYLED, "MPFX2", 1, 1, 5000, 5);
        } else {
            sautils->ScriptCommand(&scm_SHOW_TEXT_2NUMBERS_STYLED, "MPFX2", 0, 0, 5000, 5);
        }
    };

    auto button_test2 = window->AddButton(23, 2, 0);
    button_test2->onClick = []()
    {
        Remove();

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0.0f, y = 0.0f, z = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &x, &y, &z);

        int intX = (int)x;
        int intY = (int)y;
        CleoFunctions::SHOW_TEXT_2NUMBERS_STYLED("MPFX2", intX, intY, 3000, 5);

        int randomChar = CleoFunctions::GET_RANDOM_CHAR_IN_SPHERE(x, y, z, 10.0f, true, true, true);

        if(randomChar == -1)
        {
            CleoFunctions::SHOW_TEXT_2NUMBERS_STYLED("MPFX1", randomChar, 0, 3000, 5);
        } else {
            CleoFunctions::WAIT(2000, [randomChar]() {
                CleoFunctions::SHOW_TEXT_2NUMBERS_STYLED("MPFX1", randomChar, 0, 3000, 5);

                CleoFunctions::ADD_BLIP_FOR_CHAR(randomChar);
            });
        }
    };
    
    auto button_test4 = window->AddButton(23, 4, 0);
    button_test4->onClick = []()
    {
        Remove();

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0, y = 0, z = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 3.0f, 0, &x, &y, &z);

        CleoFunctions::CREATE_ACTOR_PEDTYPE(20, 284, x, y, z);
    };

    auto button_test5 = window->AddButton(23, 5, 0);
    button_test5->onClick = []()
    {
        Remove();

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0, y = 0, z = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 10, 0, &x, &y, &z);

        auto car = CleoFunctions::CREATE_CAR_AT(523, x, y, z);

        int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 4, 284);
    };
}