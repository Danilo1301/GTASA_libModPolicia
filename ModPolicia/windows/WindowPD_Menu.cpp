#include "WindowPD_Menu.h"

#include "ModConfig.h"
#include "Mod.h"
#include "PoliceDepartment.h"
#include "CleoFunctions.h"

#include "systems/Camera.h"

IWindow* WindowPD_Menu::m_Window = NULL;

extern IMenuVSL* menuVSL;

void WindowPD_Menu::Create()
{
	if (m_Window) return;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("duty");

    if(!Mod::m_Enabled)
    {
        auto button_enterDuty = window->AddButton(GetLanguageLine("enter_duty"));
        button_enterDuty->onClick = []()
        {
            Remove();
            Mod::ToggleMod(true);

            //give radio
            auto playerActor = GetPlayerActor();
            if(!Ped::PedHasWeaponId(playerActor, 10)) CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 10, 1);
        };
    } else {
        auto button_leaveDuty = window->AddButton(GetLanguageLine("leave_duty"));
        button_leaveDuty->onClick = []()
        {
            Remove();
            Mod::ToggleMod(false);
        };
    }

    auto button_editRoadblocks = window->AddButton(GetLanguageLine("edit_barriers"));
    button_editRoadblocks->onClick = []()
    {
        Remove();
        CreateEditBarrier(&Chase::m_BarrierModels[0]);
    };
    
    auto button_editSpikes = window->AddButton(GetLanguageLine("edit_spikes"));
    button_editSpikes->onClick = []()
    {
        Remove();
        CreateEditBarrier(&Chase::m_BarrierModels[1]);
    };

    auto button_editCamera = window->AddButton(GetLanguageLine("edit_bodycam"));
    button_editCamera->onClick = []()
    {
        Remove();
        CreateEditCamera();
    };

    auto button_language = window->AddButton("Change language");
    button_language->onClick = []()
    {
        Remove();
        menuVSL->ShowSelectLanguageWindow();
    };

    window->AddCheckbox(GetLanguageLine("enable_test_options"), &ModConfig::CreateTestOptionsInRadioMenu);

    auto button_close = window->AddButton(GetLanguageLine("close"));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowPD_Menu::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
}

void WindowPD_Menu::CreatePartnerMenu()
{
    if (m_Window) return;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("partner");

    auto weaponOptions = window->AddOptions(GetLanguageLine("partner_weapon"));
    for(auto weapon : PoliceDepartment::m_Weapons)
    {
        std::string weaponName = Names::GetWeaponName(weapon.weaponId);

        weaponOptions->AddOption(weapon.weaponId, weaponName);
    }
    weaponOptions->m_OptionsSelectedIndex = PoliceDepartment::m_PartnerWeaponIndex;
    
    weaponOptions->onValueChange = [weaponOptions]() {
        PoliceDepartment::m_PartnerWeaponIndex = weaponOptions->m_OptionsSelectedIndex;
    };

    auto skinOptions = window->AddOptions(GetLanguageLine("partner_skin"));
    for(auto skin : PoliceDepartment::m_PartnerSkins)
    {
        std::string skinName = Names::GetSkinName(skin.pedModelId);

        skinOptions->AddOption(skin.pedModelId, skinName);
    }
    skinOptions->m_OptionsSelectedIndex = PoliceDepartment::m_PartnerSkinIndex;

    skinOptions->onValueChange = [skinOptions]() {
        PoliceDepartment::m_PartnerSkinIndex = skinOptions->m_OptionsSelectedIndex;
    };

    auto button_spawnPed = window->AddButton(GetLanguageLine("spawn_partner"));
    button_spawnPed->onClick = []()
    {
        Remove();

        PoliceDepartment::SpawnPartner();
    };

    auto button_removeParterns = window->AddButton(GetLanguageLine("remove_partners"));
    button_removeParterns->onClick = []()
    {
        Remove();

        PoliceDepartment::RemoveAllPartners();
    };

    auto button_close = window->AddButton(GetLanguageLine("close"));
    button_close->onClick = []()
    {
        Remove();
    };
}

int GetIndexBySkinId(int id)
{
    int i = 0;
    for(auto skin : PoliceDepartment::m_PartnerSkins)
    {
        if(skin.pedModelId == id) return i;
        i++;
    }
    return 0;
}

int GetIndexByVehicleId(int id)
{
    int i = 0;
    for(auto vehicleId : PoliceDepartment::m_VehicleIds)
    {
        if(vehicleId == id) return i;
        i++;
    }
    return 0;
}

void WindowPD_Menu::CreateEditBarrier(BarrierModel* barrierModel)
{
    if (m_Window) return;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("barriers");

    // skin options

    auto skinOptions = window->AddOptions(GetLanguageLine("barrier_skin"));
    for(auto skin : PoliceDepartment::m_PartnerSkins)
    {
        std::string skinName = std::to_string(skin.pedModelId);
        skinOptions->AddOption(skin.pedModelId, skinName);
    }
    skinOptions->SetCurrentOption(barrierModel->pedModelId);

    skinOptions->onValueChange = [barrierModel, skinOptions]() {
        auto modelId = skinOptions->GetCurrentOption().value;

        barrierModel->pedModelId = modelId;
    };

    // vehicle options

    auto vehicleOptions = window->AddOptions(GetLanguageLine("barrier_vehicle"));

    for(auto vehicleId : PoliceDepartment::m_VehicleIds)
    {
        std::string vehicleName = std::to_string(vehicleId);
        vehicleOptions->AddOption(vehicleId, vehicleName);
    }
    vehicleOptions->SetCurrentOption(barrierModel->vehicleModelId);

    vehicleOptions->onValueChange = [barrierModel, vehicleOptions]() {
        barrierModel->vehicleModelId = vehicleOptions->GetCurrentOption().value;
    };

    auto button_close = window->AddButton(GetLanguageLine("close"));
    button_close->onClick = []()
    {
        Remove();

        ModConfig::SaveDataSettings();

        Create();
    };
}

void WindowPD_Menu::CreateEditCamera()
{
    // auto window = m_Window = Menu::AddWindow(6);
    // window->showPageControls = true;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "Bodycam";

    window->AddCheckbox("Enable bodycam", &Camera::m_Enabled);

    auto button_positionCamera = window->AddButton("Position");
    button_positionCamera->onClick = [window]() {
        menuVSL->AddVector2Window(window, &Camera::m_Position, -1000.0f, 1000.0f, 0.5f);
    };

    auto button_sizeCamera = window->AddButton("Size");
    button_sizeCamera->onClick = [window]() {
        menuVSL->AddVector2Window(window, &Camera::m_Size, -1000.0f, 1000.0f, 0.5f);
    };

    auto button_close = window->AddButton(GetLanguageLine("close"));
    button_close->onClick = []()
    {
        Remove();
        ModConfig::SaveSettings();
        Create();
    };
}