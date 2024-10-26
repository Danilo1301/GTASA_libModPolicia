#include "WindowCarMenu.h"

#include "Mod.h"
#include "ModConfig.h"
#include "CleoFunctions.h"
#include "PoliceDepartment.h"
#include "Debug.h"

#include "windows/WindowBackup.h"
#include "windows/WindowTrunk.h"
#include "windows/WindowPD_Menu.h"

IWindow* WindowCarMenu::m_Window = NULL;
Vehicle* WindowCarMenu::m_Vehicle = NULL;

extern IMenuVSL* menuVSL;

void WindowCarMenu::Create(Vehicle* vehicle)
{
    if(m_Window) return;

    m_Vehicle = vehicle;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("car_menu");

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

    auto equip = window->AddButton(GetLanguageLine("equip"));
    equip->onClick = []()
    {
        Remove();

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 31, 10000);
        CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 22, 10000);
        if(!Ped::PedHasWeaponId(playerActor, 10))
        {
            Debug::AddLine(1, 10);
            CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 10, 1);
        }
        CleoFunctions::CHANGE_PLAYER_MODEL_TO(0, 280);
    };

    if(vehicle->trunk->HasModelData())
    {
        auto button_configTrunk = window->AddButton(GetLanguageLine("edit_trunk"));
        button_configTrunk->onClick = []()
        {
            Remove();
            WindowTrunk::Create();
        };
    }

    auto button_configBackup = window->AddButton(GetLanguageLine("config_backup"));
    button_configBackup->onClick = [window]()
    {
        WindowBackup::CreateBackupConfig(window);
    };

    auto button_spawnPartner = window->AddButton(GetLanguageLine("spawn_partner"));
    button_spawnPartner->onClick = [vehicle]()
    {
        Remove();

        PoliceDepartment::m_SpawnParterAtPosition = GetCarPositionWithOffset(vehicle->hVehicle, CVector(-3.0f, 0, 0));
        WindowPD_Menu::CreatePartnerMenu();
    };

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