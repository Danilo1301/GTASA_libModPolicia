#include "WindowCarMenu.h"

#include "Mod.h"
#include "ModConfig.h"
#include "CleoFunctions.h"

#include "windows/WindowBackup.h"
#include "windows/WindowTrunk.h"
#include "windows/WindowPD_Menu.h"

Window* WindowCarMenu::m_Window = NULL;
Vehicle* WindowCarMenu::m_Vehicle = NULL;

void WindowCarMenu::Create(Vehicle* vehicle)
{
    if(m_Window) return;

    m_Vehicle = vehicle;

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    if(Mod::m_Enabled)
    {
        auto button_disableMod = window->AddButton(112);
        button_disableMod->onClick = []()
        {
            Remove();
            Mod::ToggleMod(false);
        };
    } else {
        auto button_enableMod = window->AddButton(111);
        button_enableMod->onClick = []()
        {
            Remove();
            Mod::ToggleMod(true);
        };
    }

    auto button_position = window->AddButton(9, 1, 0);
    button_position->onClick = [window]() {
        Remove();

        auto testWindow = Menu::AddWindow(6);
        testWindow->AddText(30);
        testWindow->AddText(30);
        testWindow->AddText(30);
        testWindow->AddText(30);

        auto positionWindow = Menu::AddPosition2DWindow(NULL, &testWindow->position, -1000.0f, 1000.0f, 0.5f, [testWindow]() {
            Window::m_DefaultWindowPosition = testWindow->position;
        });

        auto closeButton = positionWindow->AddFloatingButton(7, 0, 0, CVector2D(100, 300), CVector2D(100, 30));
        closeButton->onClick = [positionWindow, testWindow]() {
            positionWindow->RemoveThisWindow();
            testWindow->RemoveThisWindow();

            Create(m_Vehicle);
        };
    };

    auto menuWidth = window->AddFloatRange(91, &window->width, 0.0f, 1000.0f, 0.5f);
    menuWidth->onValueChange = [window]() {
        Window::m_DefaultWindowWidth = window->width;
    };

    auto equip = window->AddButton(92);
    equip->onClick = []()
    {
        Remove();

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 31, 10000);
        CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 22, 10000);
        CleoFunctions::CHANGE_PLAYER_MODEL_TO(0, 280);
    };

    if(vehicle->trunk->HasModelData())
    {
        auto button_configTrunk = window->AddButton(165);
        button_configTrunk->onClick = []()
        {
            Remove();
            WindowTrunk::Create();
        };
    }

    auto button_configBackup = window->AddButton(107);
    button_configBackup->onClick = []()
    {
        Remove();
        WindowBackup::CreateBackupConfig();
    };

    auto button_spawnPartner = window->AddButton(167);
    button_spawnPartner->onClick = []()
    {
        Remove();
        WindowPD_Menu::CreatePartnerMenu();
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        ModConfig::SaveSettings();
    };
}

void WindowCarMenu::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}