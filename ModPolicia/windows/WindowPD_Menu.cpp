#include "WindowPD_Menu.h"

#include "ModConfig.h"
#include "Mod.h"
#include "PoliceDepartment.h"
#include "CleoFunctions.h"

Window* WindowPD_Menu::m_Window = NULL;

void WindowPD_Menu::Create()
{
	if (m_Window) return;

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    if(Mod::m_Enabled)
    {
        auto button_leaveDuty = window->AddButton(142);
        button_leaveDuty->onClick = []()
        {
            Remove();
            Mod::ToggleMod(false);
        };
    } else {
        auto button_enterDuty = window->AddButton(141);
        button_enterDuty->onClick = []()
        {
            Remove();
            Mod::ToggleMod(true);
        };
    }

    window->AddCheckbox(139, &ModConfig::CreateTestOptionsInRadioMenu);

    auto button_editRoadblocks = window->AddButton(169);
    button_editRoadblocks->onClick = []()
    {
        Remove();
        CreateEditBarrier(&Chase::m_BarrierModels[0]);
    };

    auto button_editSpikes = window->AddButton(170);
    button_editSpikes->onClick = []()
    {
        Remove();
        CreateEditBarrier(&Chase::m_BarrierModels[1]);
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowPD_Menu::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}

void WindowPD_Menu::CreatePartnerMenu()
{
    if (m_Window) return;

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto weaponOptions = window->AddOptions(108);
    weaponOptions->optionsValue = PoliceDepartment::m_PartnerWeaponIndex;
    for(auto weapon : PoliceDepartment::m_Weapons)
    {
        weaponOptions->AddOption(weapon.gxtId, 0, 0);
    }
    weaponOptions->onValueChange = [weaponOptions]() {
        PoliceDepartment::m_PartnerWeaponIndex = weaponOptions->optionsValue;
    };

    auto skinOptions = window->AddOptions(159);
    skinOptions->optionsValue = PoliceDepartment::m_PartnerSkinIndex;
    for(auto skin : PoliceDepartment::m_PartnerSkins)
    {
        skinOptions->AddOption(skin.gxtId, 0, 0);
    }
    skinOptions->onValueChange = [skinOptions]() {
        PoliceDepartment::m_PartnerSkinIndex = skinOptions->optionsValue;
    };

    auto button_spawnPed = window->AddButton(148);
    button_spawnPed->onClick = []()
    {
        Remove();

        PoliceDepartment::SpawnPartner();
    };

    auto button_removeParterns = window->AddButton(149);
    button_removeParterns->onClick = []()
    {
        Remove();

        PoliceDepartment::RemoveAllPartners();
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
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
    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto skinOptions = window->AddOptions(159);
    skinOptions->optionsValue = GetIndexBySkinId(barrierModel->pedModelId);
    for(auto skin : PoliceDepartment::m_PartnerSkins)
    {
        skinOptions->AddOption(skin.gxtId, 0, 0);
    }
    skinOptions->onValueChange = [barrierModel, skinOptions]() {
        barrierModel->pedModelId = PoliceDepartment::m_PartnerSkins[skinOptions->optionsValue].pedModelId;
    };

    auto vehicleOptions = window->AddOptions(171);
    vehicleOptions->optionsValue = GetIndexByVehicleId(barrierModel->vehicleModelId);
    for(auto vehicleId : PoliceDepartment::m_VehicleIds)
    {
        vehicleOptions->AddOption(1, vehicleId, 0);
    }
    vehicleOptions->onValueChange = [barrierModel, vehicleOptions]() {
        barrierModel->vehicleModelId = PoliceDepartment::m_VehicleIds[vehicleOptions->optionsValue];
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();

        ModConfig::SaveDataSettings();

        Create();
    };
}