#include "WindowPullover.h"

#include "WindowDocument.h"
#include "WindowFrisk.h"

#include "../Pullover.h"
#include "../Scorch.h"
#include "../Log.h"
#include "../CleoFunctions.h"
#include "../Vehicles.h"


Window* WindowPullover::m_Window = NULL;

void WindowPullover::CreatePullingPed()
{
    auto ped = Pullover::m_PullingPed;
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;
    
    auto text_id = window->AddText(27, CRGBA(255, 255, 255));
    text_id->text->num1 = ped->hPed;

    /*
    auto floating_button_1 = window->AddFloatingButton(23, 3, 0, CVector2D(0, 380), CVector2D(170, 40));
    floating_button_1->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MODPMV1", 1, 2, 3, 3000, 1);
    };
    */

    auto button_pediraguardar = window->AddButton(79);
    button_pediraguardar->onClick = []()
    {
        Remove();
        Pullover::MakePedWait();
    };

    if(ped->hVehicleOwned > 0)
    {
        auto button_revistarcarro = window->AddButton(59);
        button_revistarcarro->onClick = [ped]()
        {
            Remove();

            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX63", 0, 0, 0, 3000, 1); //chegue mais perto

            Log::file << "adding wait for function" << std::endl;

            CleoFunctions::AddWaitForFunction([ped] () {
                Log::file << "testing condition" << std::endl;

                auto vehicle = Vehicles::GetVehicleByHandle(ped->hVehicleOwned);
                int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

                auto distance = Pullover::GetDistanceBetweenPedAndCar(playerActor, vehicle->hVehicle);

                Log::file << "distance from car: " << distance << std::endl;

                if(distance < Pullover::PULLOVER_MIN_DISTANCE_VEHICLE) return true;
                if(distance > Pullover::PULLOVER_MAX_DISTANCE) return true;

                return false;
            },
            [ped] () {
                Log::file << "condition executed" << std::endl;

                auto vehicle = Vehicles::GetVehicleByHandle(ped->hVehicleOwned);
                int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

                auto distance = Pullover::GetDistanceBetweenPedAndCar(playerActor, vehicle->hVehicle);

                if(distance <= Pullover::PULLOVER_MIN_DISTANCE_VEHICLE)
                {
                    Pullover::m_FriskType = FRISK_TYPE::FRISK_VEHICLE;
                    Pullover::FriskVehicle();
                } else {
                    Log::file << "Car is too far away!" << std::endl;
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX56", 0, 0, 0, 3000, 1); //muito longe
                }
            });
        };
    }
    
    if(ped->hVehicleOwned > 0)
    {
        auto button_consultarplaca = window->AddButton(67);
        button_consultarplaca->onClick = [ped]()
        {
            Remove();

            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX68", 0, 0, 0, 3000, 1); //consultar placa

            CleoFunctions::WAIT(2000, [ped]() {
                auto vehicle = Vehicles::GetVehicleByHandle(ped->hVehicleOwned);

                if(vehicle->isStolen)
                {
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX70", 0, 0, 0, 3000, 1); //produto de roubo
                } else {
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX69", 0, 0, 0, 3000, 1); //sem queixas
                }

                CleoFunctions::WAIT(1000, []() {
                    CreatePullingPed();
                });
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
                Pullover::m_FriskType = FRISK_TYPE::FRISK_PED;
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
            //CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX37", 0, 0, 0, 3000, 1); //aqui esta

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
            //CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX37", 0, 0, 0, 3000, 1); //aqui esta

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
    button_conduzir->onClick = []()
    {
        Remove();
        CreateScorchWindow();
    };

    if(ped->hVehicleOwned > 0)
    {   
        auto button_liberar = window->AddButton(57, CRGBA(170, 70, 70));
        button_liberar->onClick = []()
        {
            Remove();
            Pullover::FreeVehicle();
        };
    } else {
        auto button_close = window->AddButton(57, CRGBA(170, 70, 70));
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
    window->showPageControls = true;
    
    auto button_sairdocarro = window->AddButton(58);
    button_sairdocarro->onClick = [ped]()
    {
        Remove();

        Pullover::AskPedToLeaveCar(ped);
    };
    
    auto button_close = window->AddButton(57, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        Pullover::FreeVehicle();
    };
}

void WindowPullover::CreateScorchWindow()
{
    auto ped = Pullover::m_PullingPed;
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto button_conduzir = window->AddButton(64, 0, 0);
    button_conduzir->onClick = [ped]()
    {
        Remove();
        Scorch::StartScorchingPed(ped);
    };

    auto button_callVehicle = window->AddButton(83, 0, 0);
    button_callVehicle->onClick = [ped]()
    {
        Remove();
        Scorch::CallVehicleToScorchPed(ped);
        Log::file << "end of fn" << std::endl;
    };

    auto button_teleport = window->AddButton(84, 0, 0);
    button_teleport->onClick = [ped]()
    {
        Remove();
        Scorch::TeleportPedToPrision(ped);
    };
}

void WindowPullover::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}