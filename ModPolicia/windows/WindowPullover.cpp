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

extern IMenuVSL* menuVSL;

IWindow* WindowPullover::m_Window = NULL;

void WindowPullover::CreatePullingPed()
{
    auto ped = Pullover::m_PullingPed;
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("pullover");

    window->AddText("- Ped ID: " + std::to_string(ped->hPed), CRGBA(255, 255, 255));

    auto button_pediraguardar = window->AddButton(GetLanguageLine("ask_ped_to_wait"));
    button_pediraguardar->onClick = []()
    {
        Remove();
        Pullover::MakePedWait();
    };

    auto button_dialog = window->AddButton(GetLanguageLine("dialogs"));
    button_dialog->onClick = []()
    {
        Remove();
        CreateDialogWindow();
    };


    if(ped->hVehicleOwned > 0)
    {
        auto button_revistarcarro = window->AddButton(GetLanguageLine("frisk_car"));
        button_revistarcarro->onClick = [ped]()
        {
            Remove();

            menuVSL->ShowMessage(GetLanguageLine("get_closer"), 3000);

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

                    menuVSL->ShowMessage(GetLanguageLine("warning_too_far"), 3000);

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
        auto button_consultarplaca = window->AddButton(GetLanguageLine("check_plate"));
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

    auto button_revistar = window->AddButton(GetLanguageLine("frisk"));
    button_revistar->onClick = [playerActor, ped]()
    {   
        Remove();

        menuVSL->ShowMessage(GetLanguageLine("get_closer"), 3000);
        
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
                menuVSL->ShowMessage(GetLanguageLine("warning_too_far"), 3000);
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
        auto button_bafometro = window->AddButton(GetLanguageLine("breathalyzer"));
        button_bafometro->onClick = [ped]()
        {
            Remove();

            menuVSL->ShowMessage(GetLanguageLine("voice_begin_breathalyzer"), 2800);

            ped->UpdateBreathalyzer();

            CleoFunctions::WAIT(2500, [ped] () {
                menuVSL->ShowMessage(GetLanguageLine("breathalyzer_result", ped->breathalyzerValue), 4000);

                CreatePullingPed();
            });
        };
    }

    auto button_rg = window->AddButton(GetLanguageLine("ask_for_id"));
    button_rg->onClick = [ped]()
    {
        menuVSL->ShowMessage(GetLanguageLine("voice_ask_for_id"), 3000);
        SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/ASK_FOR_ID_", false);

        Remove();

        CleoFunctions::WAIT(3000, [ped]() {
            if(ped->HasDocuments())
            {
                menuVSL->ShowMessage(GetLanguageLine("here_it_is"), 3000);

                WindowDocument::ToggleDocuments(DOC_TYPE::RG, ped);
                WindowDocument::m_OnClose = []() {
                    CreatePullingPed();
                };
            } else {
                menuVSL->ShowMessage(GetLanguageLine("i_forgot_home"), 3000);

                CreatePullingPed();
            }
        });
    };

    auto button_cnh = window->AddButton(GetLanguageLine("ask_for_drivers_license"));
    button_cnh->onClick = [ped]()
    {
        menuVSL->ShowMessage(GetLanguageLine("voice_ask_for_drivers_license"), 3000);
        SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/ASK_FOR_DRIVERS_LICENSE_", false);

        Remove();

        CleoFunctions::WAIT(3000, [ped]() {
            if(ped->HasDocuments())
            {
                menuVSL->ShowMessage(GetLanguageLine("here_it_is"), 3000);

                WindowDocument::ToggleDocuments(DOC_TYPE::CNH, ped);
                WindowDocument::m_OnClose = []() {
                    CreatePullingPed();
                };
            } else {
                menuVSL->ShowMessage(GetLanguageLine("i_forgot_home"), 3000);
                CreatePullingPed();
            }
        });
    };

    if(ped->hVehicleOwned > 0)
    {
        auto button_ticket = window->AddButton(GetLanguageLine("issue_ticket"));
        button_ticket->onClick = [ped]()
        {
            Remove();
            WindowTicket::Create();
        };
    }

    if(ped->hVehicleOwned > 0)
    {
        auto button_guincho = window->AddButton(GetLanguageLine("call_tow_truck"));
        button_guincho->onClick = [ped]()
        {
            if(!CleoFunctions::CAR_DEFINED(ped->hVehicleOwned))
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Tried to call tow truck to a not defined vehicle" << std::endl;
                return;
            }

            SoundSystem::PlayHTAudio();
            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/REQUEST_TOW_TRUCK_", false);
            menuVSL->ShowMessage(GetLanguageLine("request_tow_truck"), 3000);

            Remove();
            Scorch::CallTowTruckToVehicle(Vehicles::GetVehicleByHandle(ped->hVehicleOwned));
        };
    }

    auto button_conduzir = window->AddButton(GetLanguageLine("scorch_criminal"));
    button_conduzir->onClick = []()
    {
        Remove();
        CreateScorchWindow();
    };

    auto button_close = window->AddButton(GetLanguageLine("free_from_pullover"));
    button_close->onClick = [ped]()
    {
        if(ped->hVehicleOwned > 0)
        {
            Remove();
            Pullover::FreeVehicle();
        } else {
            Remove();
            Pullover::FreePed();
        }
    };
}

void WindowPullover::CreatePullingCar()
{
    auto vehicle = Pullover::m_PullingVehicle;
    auto ped = Pullover::m_PullingPed;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("pullover");
    
    auto button_sairdocarro = window->AddButton(GetLanguageLine("ask_to_leave_car"));
    button_sairdocarro->onClick = [vehicle]()
    {
        Remove();
        Pullover::AskPedsToLeaveCar(vehicle);
    };
    
    auto button_stopOnRight = window->AddButton(GetLanguageLine("ask_to_park_right"));
    button_stopOnRight->onClick = [vehicle]()
    {
        Remove();
        Pullover::AskPedToStopCarOnRight(vehicle);
    };

    auto button_close = window->AddButton(GetLanguageLine("free_from_pullover"));
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

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("scorch_criminal");

    auto button_callVehicle = window->AddButton(GetLanguageLine("call_a_police_car"));
    button_callVehicle->onClick = [ped]()
    {
        Remove();
        Scorch::CallVehicleToScorchPed(ped);
    };

    auto button_conduzir = window->AddButton(GetLanguageLine("scorch_by_yourself"));
    button_conduzir->onClick = [ped]()
    {
        Remove();
        Scorch::CarryPed(ped);
        Scorch::ToggleCarryWindow(true);
    };

    auto button_teleport = window->AddButton(GetLanguageLine("teleport"));
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

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("dialog");
    window->m_Width = 500.0f;
    
    for(auto pair : Dialogs::m_Dialogs)
    {
        auto id = pair.first;
        auto dialog = &Dialogs::m_Dialogs[id];

        int responseId = ped->GetDialogResponse(id);

        std::string extraText = "";

        if(id == eDialogId::DIALOG_VEHICLE_OWNER)
        {
            if(!vehicle) continue;

            responseId = GetRandomNumber(0, 1);

            if(vehicle->isStolen) responseId = GetRandomNumber(0, 2);
        }
        if(id == eDialogId::DIALOG_CRIMES)
        {
            if(ped->crimeCodes.size() > 0)
            {
                responseId = 1;

                std::vector<std::string> strs;
                for(auto code : ped->crimeCodes) strs.push_back(std::to_string(code));

                extraText = joinString(strs);
            } else {
                responseId = 0;
            }
        }
        if(id == eDialogId::DIALOG_ARRESTED)
        {
            if(ped->crimeCodes.size() > 0)
            {
                responseId = 1; //yes
            } else {
                responseId = 0; //no
            }
        }
        if(id == eDialogId::DIALOG_ILLEGAL_STUFF_IN_CAR)
        {
            if(!vehicle) continue;

            if(vehicle->HasIlegalStuff())
            {
                responseId = 1; //yes
            } else {
                responseId = 0; //no
            }
        }

        if(responseId == -1)
        {
            switch (id)
            {
            case eDialogId::DIALOG_CRIMES:
                break;
            default:
                responseId = GetRandomNumber(0, dialog->responses.size() - 1);
                break;
            }  

            ped->dialogResponses[id] = responseId;
        }

        // response

        std::string response = GetLanguageLine(dialog->responses[responseId]);

        if(extraText.size() > 0)
        {
            response = GetLanguageLine(dialog->responses[responseId], extraText.c_str());
        }

        //

        auto button_question = window->AddButton(GetLanguageLine(dialog->question));
        button_question->onClick = [dialog, response]()
        {
            Remove();

            menuVSL->ShowMessage(GetLanguageLine("dialog_officer") + " ~w~" + GetLanguageLine(dialog->question), 3000);
            //CleoFunctions::SHOW_TEXT_3NUMBERS(dialog->questionGxt, 0, 0, 0, 3000, 1);

            CleoFunctions::WAIT(3000, [dialog, response] () {
                
                menuVSL->ShowMessage(GetLanguageLine("dialog_ped") + " ~w~" + response, 3000);
                //CleoFunctions::SHOW_TEXT_3NUMBERS(dialog->responses[response], num1, num2, 0, 3000, 1);

                CleoFunctions::WAIT(3000, [] () {
                    CreateDialogWindow();
                });
            });
        };
    }

    auto button_back = window->AddButton(GetLanguageLine("back"));
    button_back->onClick = [ped]()
    {
        Remove();
        CreatePullingPed();
    };
}

void WindowPullover::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
}