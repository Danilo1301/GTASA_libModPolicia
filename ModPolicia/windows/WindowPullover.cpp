#include "WindowPullover.h"

#include "WindowDocument.h"
#include "WindowFrisk.h"

#include "../Pullover.h"
#include "../Log.h"
#include "../CleoFunctions.h"


Window* WindowPullover::m_Window = NULL;

void WindowPullover::CreatePullingPed()
{
    auto ped = Pullover::m_PullingPed;
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    auto window = m_Window = Menu::AddWindow(6);
    window->position = CVector2D(200, 200); //80, 200
    window->showPageControls = true;
    
    auto text_id = window->AddText(27, CRGBA(255, 255, 255));
    text_id->text->num1 = ped->hPed;

    auto floating_button_1 = window->AddFloatingButton(23, 3, 0, CVector2D(0, 380), CVector2D(170, 40));
    floating_button_1->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MODPMV1", 1, 2, 3, 3000, 1);
    };

    if(ped->vehicleOwned)
    {
        auto button_revistarcarro = window->AddButton(59);
        button_revistarcarro->onClick = [ped]()
        {
            Remove();

            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX63", 0, 0, 0, 3000, 1); //chegue mais perto

            Log::file << "adding wait for function" << std::endl;

            CleoFunctions::AddWaitForFunction([ped] () {
                Log::file << "testing condition" << std::endl;

                auto vehicle = ped->vehicleOwned;
                int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

                auto distance = Pullover::GetDistanceBetweenPedAndCar(playerActor, vehicle->hVehicle);

                Log::file << "distance from car: " << distance << std::endl;

                if(distance < Pullover::PULLOVER_MIN_DISTANCE_VEHICLE) return true;
                if(distance > Pullover::PULLOVER_MAX_DISTANCE) return true;

                return false;
            },
            [ped] () {
                Log::file << "condition executed" << std::endl;

                auto vehicle = ped->vehicleOwned;
                int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

                auto distance = Pullover::GetDistanceBetweenPedAndCar(playerActor, vehicle->hVehicle);

                if(distance <= Pullover::PULLOVER_MIN_DISTANCE_VEHICLE)
                {
                    Pullover::m_FriskType = PULLOVER_TYPE::PULLING_VEHICLE;
                    Pullover::FriskVehicle();
                } else {
                    Log::file << "Car is too far away!" << std::endl;
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX56", 0, 0, 0, 3000, 1); //muito longe
                }
            });
        };
    }

    auto button_revistar = window->AddButton(33, 1, 0);
    button_revistar->onClick = [playerActor, ped]()
    {   
        Remove();

        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX63", 0, 0, 0, 3000, 1); //chegue mais perto
        
        Log::file << "adding wait for function" << std::endl;

        CleoFunctions::AddWaitForFunction([playerActor, ped] () {
            Log::file << "testing condition" << std::endl;

            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) return false;

            auto distance = Pullover::GetDistanceBetweenPeds(playerActor, ped->hPed);

            Log::file << "distance from ped: " << distance << std::endl;

            if(distance < Pullover::PULLOVER_MIN_DISTANCE_PED) return true;
            if(distance > Pullover::PULLOVER_MAX_DISTANCE) return true;

            return false;
        },
        [playerActor, ped] () {
            Log::file << "condition executed" << std::endl;

            auto distance = Pullover::GetDistanceBetweenPeds(playerActor, ped->hPed);

            if(distance <= Pullover::PULLOVER_MIN_DISTANCE_PED)
            {
                Pullover::m_FriskType = PULLOVER_TYPE::PULLING_PED;
                Pullover::FriskPed();
            } else {
                Log::file << "Ped is too far away!" << std::endl;
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX56", 0, 0, 0, 3000, 1); //muito longe
            }
        });
    };

    auto button_rg = window->AddButton(34, 0, 0);
    button_rg->onClick = [ped]()
    {
        if(ped->HasDocuments())
        {
            //CleoFunctions::SHOW_TEXT_3NUMBERS("MODPMV1", ped->birthDay, ped->birthMonth, ped->birthYear, 3000, 1);

            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX37", 0, 0, 0, 3000, 1); //aqui esta

            Remove();

            WindowDocument::ToggleDocuments(DOC_TYPE::RG, ped);
            WindowDocument::m_OnClose = []() {
                CreatePullingPed();
            };
        } else {
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX36", 0, 0, 0, 3000, 1); //esqueci em casa
        }
    };

    auto button_cnh = window->AddButton(44, 0, 0);
    button_cnh->onClick = [ped]()
    {
        if(ped->HasDocuments())
        {
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX37", 0, 0, 0, 3000, 1); //aqui esta

            Remove();

            WindowDocument::ToggleDocuments(DOC_TYPE::CNH, ped);
            WindowDocument::m_OnClose = []() {
                CreatePullingPed();
            };
        } else {
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX36", 0, 0, 0, 3000, 1); //esqueci em casa
        }
    };

    auto button_conduzir = window->AddButton(64, 0, 0);
    button_conduzir->onClick = [playerActor, ped]()
    {
        Remove();

        Pullover::m_ScorchingPed = ped;

        if(ped->vehicleOwned)
        {
            if(ped->vehicleOwned->blip > 0)
            {
                CleoFunctions::DISABLE_MARKER(ped->vehicleOwned->blip);
                ped->vehicleOwned->blip = 0;
            }
        }

        Log::file << "Conduzir para a DP" << std::endl;

        /*
        01C2: remove_references_to_actor 4@

        07AF: 6@ = player $PLAYER_CHAR group 

        //062F: 6@ = create_group_type 0 //is this really encessary    
        0630: put_actor $PLAYER_ACTOR in_group 4@ as_leader 
        0631: put_actor 4@ in_group 6@
        */
        CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(ped->hPed);
            
        auto playerGroup = CleoFunctions::GET_PLAYER_GROUP(0);

        Log::file << "playerGroup = " << playerGroup << std::endl;

        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        /*
        CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(playerActor, ped->hPed);
        CleoFunctions::PUT_ACTOR_IN_GROUP(ped->hPed, playerGroup);
        */

        CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(ped->hPed, playerActor);
        CleoFunctions::PUT_ACTOR_IN_GROUP(playerGroup, ped->hPed);

        Log::file << "create sphere" << std::endl;

        Pullover::m_ScorchingPedSphere = CleoFunctions::CREATE_SPHERE(1536.1325, -1671.2093, 13.3828, 3.0);

        Log::file << "sphere = " << Pullover::m_ScorchingPedSphere << std::endl;
    };

    if(ped->vehicleOwned)
    {   
        auto button_liberar = window->AddButton(57, CRGBA(170, 70, 70));
        button_liberar->onClick = []()
        {
            Remove();
            Pullover::FreeVehicle();
        };
    } else {
        auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
        button_close->onClick = []()
        {
            Remove();
            Pullover::FreePed();
        };
    }
    
}

void WindowPullover::CreatePullingCar()
{
    auto vehicle = Pullover::m_PullingVehicle;
    auto ped = Pullover::m_PullingPed;

    auto window = m_Window = Menu::AddWindow(6);
    window->position = CVector2D(200, 200); //80, 200
    window->showPageControls = true;
    
    auto button_sairdocarro = window->AddButton(58);
    button_sairdocarro->onClick = [ped]()
    {
        Remove();
        /*
        0633: AS_actor 21@ exit_car
        wait 1000 
        0812: AS_actor 21@ perform_animation "handsup" IFP "PED" framedelta 4.0 loopA 0 lockX 0 lockY 0 lockF 1 time -1 // versionB 
        */
        CleoFunctions::EXIT_CAR_AS_ACTOR(ped->hPed);
        
        CleoFunctions::WAIT(1000, [ped]() {
            CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(ped->hPed, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1);
        });

        CleoFunctions::WAIT(2000, []() {
            CreatePullingPed();
        });
    };
    
    auto button_close = window->AddButton(57, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        Pullover::FreeVehicle();
    };
}

void WindowPullover::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}