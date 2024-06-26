#include "WindowPullover.h"

#include "WindowDocument.h"
#include "WindowFrisk.h"
#include "WindowTicket.h"

#include "../Pullover.h"
#include "../Scorch.h"
#include "../Log.h"
#include "../CleoFunctions.h"
#include "../Vehicles.h"
#include "../SoundSystem.h"

Window* WindowPullover::m_Window = NULL;

void WindowPullover::CreatePullingPed()
{
    auto ped = Pullover::m_PullingPed;
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;
    
    auto text_id = window->AddText(27);
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

            CleoFunctions::AddCondition([ped] (std::function<void()> complete, std::function<void()> cancel) {
                if(!CleoFunctions::ACTOR_DEFINED(ped->hPed))
                {
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped is not defined anymore" << std::endl;
                    cancel();
                    return;
                }

                if(!CleoFunctions::CAR_DEFINED(ped->hVehicleOwned))
                {
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "Car owned is not defined anymore" << std::endl;
                    cancel();
                    return;
                }

                auto vehicle = Vehicles::GetVehicleByHandle(ped->hVehicleOwned);
                auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

                auto distance = Pullover::GetDistanceBetweenPedAndCar(playerActor, vehicle->hVehicle);

                if(distance > Pullover::PULLOVER_MAX_DISTANCE)
                {
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "Car is too far away!" << std::endl;
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX56", 0, 0, 0, 3000, 1); //muito longe
                    cancel();
                    return;
                }

                if(distance < Pullover::PULLOVER_MIN_DISTANCE_VEHICLE)
                {
                    complete();
                    return;
                }
            }, []() {
                Pullover::m_FriskType = FRISK_TYPE::FRISK_VEHICLE;
                Pullover::FriskVehicle();
            }, []() {
                Pullover::m_PullingVehicle = NULL;
                Pullover::m_PullingPed = NULL;
            });
        };
    }
    
    if(ped->hVehicleOwned > 0)
    {
        auto button_consultarplaca = window->AddButton(67);
        button_consultarplaca->onClick = [ped]()
        {
            Remove();

            SoundSystem::PlayHTAudio();
            SoundSystem::PlayStreamFromAudiosFolder("voices/CHECK_VEHICLE_PLATE.wav", false);
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX68", 0, 0, 0, 3000, 1); //consultar placa

            CleoFunctions::WAIT(4000, [ped]() {
                auto vehicle = Vehicles::GetVehicleByHandle(ped->hVehicleOwned);

                if(vehicle->isStolen)
                {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolder("voices/VEHICLE_PLATE_STOLEN.wav", false);
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX70", 0, 0, 0, 3000, 1); //produto de roubo
                } else {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolder("voices/VEHICLE_PLATE_OK.wav", false);
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
        
        Log::Level(LOG_LEVEL::LOG_BOTH) << "waiting to get closer to the ped" << std::endl;

        CleoFunctions::AddCondition([playerActor, ped] (std::function<void()> complete, std::function<void()> cancel) {
            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) return;

            if(!CleoFunctions::ACTOR_DEFINED(ped->hPed))
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped is not defined anymore" << std::endl;
                cancel();
                return;
            }

            auto distance = Pullover::GetDistanceBetweenPeds(playerActor, ped->hPed);

            Log::Level(LOG_LEVEL::LOG_BOTH) << "distance from ped: " << distance << std::endl;

            if(distance > Pullover::PULLOVER_MAX_DISTANCE)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped is too far away!" << std::endl;
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX56", 0, 0, 0, 3000, 1); //muito longe
                cancel();
                return;
            }

            if(distance < Pullover::PULLOVER_MIN_DISTANCE_PED)
            {
                complete();
                return;
            }
        }, []() {
            Pullover::m_FriskType = FRISK_TYPE::FRISK_PED;
            Pullover::FriskPed();
        }, []() {
            Pullover::m_PullingVehicle = NULL;
            Pullover::m_PullingPed = NULL;
        });
    };

    if(ped->hVehicleOwned > 0)
    {
        auto button_bafometro = window->AddButton(126, 0, 0);
        button_bafometro->onClick = [ped]()
        {
            Remove();

            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX130", 0, 0, 0, 2500, 1); //vou realizar teste

            ped->UpdateBreathalyzer();

            CleoFunctions::WAIT(2500, [ped] () {
                int value = (int)(ped->breathalyzerValue * 100.0f);

                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX131", value, 0, 0, 4000, 1); //resultado

                CreatePullingPed();
            });
        };
    }

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

    if(ped->hVehicleOwned > 0)
    {
        auto button_ticket = window->AddButton(99, 0, 0);
        button_ticket->onClick = [ped]()
        {
            Remove();
            WindowTicket::Create();
        };
    }

    if(ped->hVehicleOwned > 0)
    {
        auto button_guincho = window->AddButton(109, 0, 0);
        button_guincho->onClick = [ped]()
        {
            if(!CleoFunctions::CAR_DEFINED(ped->hVehicleOwned))
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Tried to call tow truck to a not defined vehicle" << std::endl;
                return;
            }

            SoundSystem::PlayHTAudio();
            SoundSystem::PlayStreamFromAudiosFolder("voices/REQUEST_TOW_TRUCK.wav", false);
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX110", 0, 0, 0, 3000, 1); //solicito guincho

            Remove();
            Scorch::CallTowTruckToVehicle(Vehicles::GetVehicleByHandle(ped->hVehicleOwned));
        };
    }

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
    button_sairdocarro->onClick = [vehicle]()
    {
        Remove();
        Pullover::AskPedsToLeaveCar(vehicle);
    };
    
    auto button_stopOnRight = window->AddButton(161);
    button_stopOnRight->onClick = [vehicle]()
    {
        Remove();
        Pullover::AskPedToStopCarOnRight(vehicle);
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

    auto button_callVehicle = window->AddButton(83, 0, 0);
    button_callVehicle->onClick = [ped]()
    {
        Remove();
        Scorch::CallVehicleToScorchPed(ped);
    };

    auto button_conduzirPortaMalas = window->AddButton(164, 0, 0);
    button_conduzirPortaMalas->onClick = [ped]()
    {
        Remove();
        Scorch::CarryPed(ped);
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