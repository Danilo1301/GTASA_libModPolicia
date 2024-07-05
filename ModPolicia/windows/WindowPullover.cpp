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
#include "systems/Dialog.h"

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

    auto button_dialog = window->AddButton(174, 0, 0);
    button_dialog->onClick = [ped]()
    {
        Remove();
        CreateDialogWindow();
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

            Pullover::CheckVehiclePlate(ped->hVehicleOwned, [] () {
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
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX35", 0, 0, 0, 3000, 1); //me ve o RG
        SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/ASK_FOR_ID_", false);

        Remove();

        CleoFunctions::WAIT(3000, [ped]() {
            if(ped->HasDocuments())
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX37", 0, 0, 0, 3000, 1); //aqui esta

                WindowDocument::ToggleDocuments(DOC_TYPE::RG, ped);
                WindowDocument::m_OnClose = []() {
                    CreatePullingPed();
                };
            } else {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX36", 0, 0, 0, 3000, 1); //esqueci em casa
                CreatePullingPed();
            }
        });
    };

    auto button_cnh = window->AddButton(44, 0, 0);
    button_cnh->onClick = [ped]()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX173", 0, 0, 0, 3000, 1); //me ve a CNH
        SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/ASK_FOR_DRIVERS_LICENSE_", false);

        Remove();

        CleoFunctions::WAIT(3000, [ped]() {
            if(ped->HasDocuments())
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX37", 0, 0, 0, 3000, 1); //aqui esta

                WindowDocument::ToggleDocuments(DOC_TYPE::CNH, ped);
                WindowDocument::m_OnClose = []() {
                    CreatePullingPed();
                };
            } else {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX36", 0, 0, 0, 3000, 1); //esqueci em casa
                CreatePullingPed();
            }
        });
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
            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/REQUEST_TOW_TRUCK_", false);
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

    auto button_conduzir = window->AddButton(64, 0, 0);
    button_conduzir->onClick = [ped]()
    {
        Remove();
        Scorch::CarryPed(ped);
        Scorch::ToggleCarryWindow(true);
    };

    /*
    auto button_conduzirPortaMalas = window->AddButton(164, 0, 0);
    button_conduzirPortaMalas->onClick = [ped]()
    {
        Remove();
        Scorch::CarryPed(ped);
    };
    */

    auto button_teleport = window->AddButton(84, 0, 0);
    button_teleport->onClick = [ped]()
    {
        Remove();
        Scorch::TeleportPedToPrision(ped);
    };
}

void WindowPullover::CreateDialogWindow()
{
    auto vehicle = Pullover::m_PullingVehicle;
    auto ped = Pullover::m_PullingPed;

    auto window = m_Window = Menu::AddWindow(6);
    window->position = CVector2D(150, 200);
    window->showPageControls = true;
    window->width = 400.0f;
    
    for(auto pair : Dialogs::m_Dialogs)
    {
        auto id = pair.first;
        auto dialog = &Dialogs::m_Dialogs[id];

        int response = ped->GetDialogResponse(id);
        int num1 = 0;
        int num2 = 0;

        if(id == eDialogId::DIALOG_VEHICLE_OWNER)
        {
            if(!vehicle) continue;

            response = GetRandomNumber(0, 1);

            if(vehicle->isStolen) response = GetRandomNumber(0, 2);
        }
        if(id == eDialogId::DIALOG_CRIMES)
        {
            response = 0;
            if(ped->crimeCodes.size() == 1)
            {
                response = 1;
                num1 = ped->crimeCodes[0];
            }
            if(ped->crimeCodes.size() == 2)
            {
                response = 2;
                num1 = ped->crimeCodes[0];
                num2 = ped->crimeCodes[1];
            }
        }
        if(id == eDialogId::DIALOG_ARRESTED)
        {
            if(ped->crimeCodes.size() > 0)
            {
                response = 1; //yes
            } else {
                response = 0; //no
            }
        }
        if(id == eDialogId::DIALOG_ILLEGAL_STUFF_IN_CAR)
        {
            if(!vehicle) continue;

            if(vehicle->HasIlegalStuff())
            {
                response = 1; //yes
            } else {
                response = 0; //no
            }
        }

        if(response == -1)
        {
            switch (id)
            {
            case eDialogId::DIALOG_CRIMES:
                break;
            default:
                response = GetRandomNumber(0, dialog->responses.size() - 1);
                break;
            }  

            ped->dialogResponses[id] = response;
        }

        auto button_question = window->AddButton(dialog->gxtId);
        button_question->onClick = [dialog, response, num1, num2]()
        {
            Remove();

            CleoFunctions::SHOW_TEXT_3NUMBERS(dialog->gxtId, 0, 0, 0, 3000, 1);

            CleoFunctions::WAIT(3000, [dialog, response, num1, num2] () {

                CleoFunctions::SHOW_TEXT_3NUMBERS(dialog->responses[response], num1, num2, 0, 3000, 1);

                CleoFunctions::WAIT(3000, [] () {
                    CreateDialogWindow();
                });
            });
        };
    }

    auto button_close = window->AddButton(8, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        CreatePullingPed();
    };
}

void WindowPullover::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}