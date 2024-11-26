#include "WindowCarMenu.h"

#include "Mod.h"
#include "Log.h"
#include "ModConfig.h"
#include "CleoFunctions.h"
#include "PoliceDepartment.h"
#include "Debug.h"
#include "Pullover.h"
#include "Scorch.h"
#include "SoundSystem.h"

#include "systems/PlaceObjects.h"

#include "WindowBackup.h"
#include "WindowTrunk.h"
#include "WindowPD_Menu.h"
#include "WindowTicket.h"

IWindow* WindowCarMenu::m_Window = NULL;
Vehicle* WindowCarMenu::m_Vehicle = NULL;

extern IMenuVSL* menuVSL;

void WindowCarMenu::Create(Vehicle* vehicle)
{
    if(m_Window) return;

    m_Vehicle = vehicle;

    auto isPoliceCar = vehicle->IsPoliceCar();
    auto isCarBeeinPulledOver = vehicle == Pullover::m_PullingVehicle;
    
    Ped* driver = NULL;
    if(Pullover::m_PullingPed) driver = Pullover::m_PullingPed;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("car_menu");
    window->m_Position = ModConfig::MenuDefaultPosition;

    if(isPoliceCar)
    {
        if(Mod::m_Enabled)
        {
            auto button_disableMod = window->AddButton(GetLanguageLine("disable_mod"));
            button_disableMod->onClick = []()
            {
                Remove();
                Mod::ToggleMod(false);
            };
        } else {
            auto button_enableMod = window->AddButton(GetLanguageLine("enable_mod"));
            button_enableMod->onClick = []()
            {
                Remove();
                Mod::ToggleMod(true);
            };
        }
    }

    // auto button_position = window->AddButton(GetLanguageLine("edit_position"));
    // button_position->onClick = [window]() {
    //     Remove();

    //     auto testWindow = Menu::AddWindow(6);
    //     testWindow->AddText(30);
    //     testWindow->AddText(30);
    //     testWindow->AddText(30);
    //     testWindow->AddText(30);

    //     auto positionWindow = Menu::AddPosition2DWindow(NULL, &testWindow->position, -1000.0f, 1000.0f, 0.5f, [testWindow]() {
    //         Window::m_DefaultWindowPosition = testWindow->position;
    //     });

    //     auto closeButton = positionWindow->AddFloatingButton(7, 0, 0, CVector2D(100, 300), CVector2D(100, 30));
    //     closeButton->onClick = [positionWindow, testWindow]() {
    //         positionWindow->RemoveThisWindow();
    //         testWindow->RemoveThisWindow();

    //         Create(m_Vehicle);
    //     };
    // };

    // auto menuWidth = window->AddFloatRange(91, &window->width, 0.0f, 1000.0f, 0.5f);
    // menuWidth->onValueChange = [window]() {
    //     Window::m_DefaultWindowWidth = window->width;
    // };

    if(isPoliceCar)
    {
        auto equip = window->AddButton(GetLanguageLine("equip"));
        equip->onClick = []()
        {
            Remove();

            int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

            CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 31, 10000);
            CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 22, 10000);
            if(!Ped::PedHasWeaponId(playerActor, 10))
            {
                CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 10, 1);
            }
            CleoFunctions::CHANGE_PLAYER_MODEL_TO(0, 280);
        };
    }

    if(vehicle->trunk->HasModelData())
    {
        auto button_configTrunk = window->AddButton(GetLanguageLine("edit_trunk"));
        button_configTrunk->onClick = []()
        {
            Remove();
            WindowTrunk::Create();
        };
    }

    if(isPoliceCar)
    {
        auto button_configBackup = window->AddButton(GetLanguageLine("config_backup"));
        button_configBackup->onClick = [window]()
        {
            WindowBackup::CreateBackupConfig(window);
        };
    }

    if(isPoliceCar)
    {
        auto button_spawnPartner = window->AddButton(GetLanguageLine("spawn_partner"));
        button_spawnPartner->onClick = [vehicle]()
        {
            Remove();

            PoliceDepartment::m_SpawnParterAtPosition = GetCarPositionWithOffset(vehicle->hVehicle, CVector(-3.0f, 0, 0));
            WindowPD_Menu::CreatePartnerMenu();
        };
    }

    if(isCarBeeinPulledOver)
    {
        auto button_sairdocarro = window->AddButton(GetLanguageLine("ask_to_leave_car"));
        button_sairdocarro->onClick = [vehicle]()
        {
            Remove();
            Pullover::AskPedsToLeaveCar(vehicle);
        };
    }
    
    if(isCarBeeinPulledOver)
    {
        auto button_stopOnRight = window->AddButton(GetLanguageLine("ask_to_park_right"));
        button_stopOnRight->onClick = [vehicle]()
        {
            Remove();
            Pullover::AskPedToStopCarOnRight(vehicle);
        };
    }

    
    auto button_revistarcarro = window->AddButton(GetLanguageLine("frisk_car"));
    button_revistarcarro->onClick = [vehicle]()
    {
        Remove();

        menuVSL->ShowMessage(GetLanguageLine("get_closer"), 3000);

        CleoFunctions::AddCondition([vehicle] (std::function<void()> complete, std::function<void()> cancel) {
       
            if(!CleoFunctions::CAR_DEFINED(vehicle->hVehicle))
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle is not defined anymore" << std::endl;
                cancel();
                return;
            }

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
        }, [vehicle]() {
            Pullover::FriskVehicle(vehicle);
        }, []() {
            Pullover::m_PullingVehicle = NULL;
            Pullover::m_PullingPed = NULL;
        });
    };
    
    auto button_consultarplaca = window->AddButton(GetLanguageLine("check_plate"));
    button_consultarplaca->onClick = [vehicle]()
    {
        Remove();

        Pullover::CheckVehiclePlate(vehicle->hVehicle, [] () {
            CleoFunctions::WAIT(1000, []() {
                Create(WindowCarMenu::m_Vehicle);
                //CreatePullingPed();
            });
        });
    };

    auto button_ticket = window->AddButton(GetLanguageLine("issue_ticket"));
    button_ticket->onClick = []()
    {
        Remove();
        WindowTicket::Create();
    };
    
    
    auto button_guincho = window->AddButton(GetLanguageLine("call_tow_truck"));
    button_guincho->onClick = [vehicle]()
    {
        // if(!CleoFunctions::CAR_DEFINED(ped->hVehicleOwned))
        // {
        //     Log::Level(LOG_LEVEL::LOG_BOTH) << "Tried to call tow truck to a not defined vehicle" << std::endl;
        //     return;
        // }

        SoundSystem::PlayHTAudio();
        SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/REQUEST_TOW_TRUCK_", false);
        menuVSL->ShowMessage(GetLanguageLine("request_tow_truck"), 3000);

        Remove();
        Scorch::CallTowTruckToVehicle(vehicle);
    };

    if(isCarBeeinPulledOver)
    {
        // auto button_close = window->AddButton(GetLanguageLine("free_from_pullover"));
        // button_close->onClick = []()
        // {
        //     Remove();
        //     Pullover::FreeVehicle();
        // };
    } else {
        
    }

    if(isPoliceCar)
    {
        auto button_cone = window->AddButton("Objects");
        button_cone->onClick = []()
        {
            Remove();
            PlaceObjects::StartSelectingObjects();
        };
    }

    auto button_close = window->AddButton(GetLanguageLine("close"));
    button_close->onClick = []()
    {
        Remove();
        ModConfig::SaveSettings();
    };
}

void WindowCarMenu::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
}