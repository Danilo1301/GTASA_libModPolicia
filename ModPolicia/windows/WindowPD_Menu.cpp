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
    window->m_Title = "Servico";

    if(!Mod::m_Enabled)
    {
        auto button_enterDuty = window->AddButton("Entrar em servico");
        button_enterDuty->onClick = []()
        {
            Remove();
            Mod::ToggleMod(true);

            //give radio
            auto playerActor = GetPlayerActor();
            if(!Ped::PedHasWeaponId(playerActor, 10)) CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 10, 1);
        };
    } else {
        auto button_leaveDuty = window->AddButton("Sair do servico");
        button_leaveDuty->onClick = []()
        {
            Remove();
            Mod::ToggleMod(false);
        };
    }

    window->AddCheckbox("Ativar opcoes teste", &ModConfig::CreateTestOptionsInRadioMenu);

    auto button_editRoadblocks = window->AddButton("Editar barreiras");
    button_editRoadblocks->onClick = []()
    {
        Remove();
        CreateEditBarrier(&Chase::m_BarrierModels[0]);
    };
    
    auto button_editSpikes = window->AddButton("Editar espinhos");
    button_editSpikes->onClick = []()
    {
        Remove();
        CreateEditBarrier(&Chase::m_BarrierModels[1]);
    };

    auto button_editCamera = window->AddButton("Editar camera");
    button_editCamera->onClick = []()
    {
        Remove();
        CreateEditCamera();
    };

    auto button_close = window->AddButton("~r~Fechar");
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
    window->m_Title = "Parceiro";

    auto weaponOptions = window->AddOptions("ID da arma");
    for(auto weapon : PoliceDepartment::m_Weapons)
    {
        std::string weaponName = std::to_string(weapon.weaponId);

        weaponOptions->AddOption(weapon.weaponId, weaponName);
    }
    weaponOptions->m_OptionsSelectedIndex = PoliceDepartment::m_PartnerWeaponIndex;
    
    weaponOptions->onValueChange = [weaponOptions]() {
        PoliceDepartment::m_PartnerWeaponIndex = weaponOptions->m_OptionsSelectedIndex;
    };

    auto skinOptions = window->AddOptions("ID da skin");
    for(auto skin : PoliceDepartment::m_PartnerSkins)
    {
        std::string skinName = std::to_string(skin.pedModelId);

        skinOptions->AddOption(skin.pedModelId, skinName);
    }
    skinOptions->m_OptionsSelectedIndex = PoliceDepartment::m_PartnerSkinIndex;

    skinOptions->onValueChange = [skinOptions]() {
        PoliceDepartment::m_PartnerSkinIndex = skinOptions->m_OptionsSelectedIndex;
    };

    auto button_spawnPed = window->AddButton("Spawnar parceiro");
    button_spawnPed->onClick = []()
    {
        Remove();

        PoliceDepartment::SpawnPartner();
    };

    auto button_removeParterns = window->AddButton("Remover parceiros");
    button_removeParterns->onClick = []()
    {
        Remove();

        PoliceDepartment::RemoveAllPartners();
    };

    auto button_close = window->AddButton("~r~Fechar");
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
    window->m_Title = "Barreiras";

    // skin options

    auto skinOptions = window->AddOptions("Skin");
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

    auto vehicleOptions = window->AddOptions("Veiculo");

    for(auto vehicleId : PoliceDepartment::m_VehicleIds)
    {
        std::string vehicleName = std::to_string(vehicleId);
        vehicleOptions->AddOption(vehicleId, vehicleName);
    }
    vehicleOptions->SetCurrentOption(barrierModel->vehicleModelId);

    vehicleOptions->onValueChange = [barrierModel, vehicleOptions]() {
        barrierModel->vehicleModelId = vehicleOptions->GetCurrentOption().value;
    };

    auto button_close = window->AddButton("~r~Fechar");
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

    // window->AddCheckbox(216, &Camera::m_Enabled);

    window->AddCheckbox("Ativar bodycam", &Camera::m_Enabled);

    // auto button_positionCamera = window->AddButton(9, 0, 0);
    // button_positionCamera->onClick = [window]() {
    //     Menu::AddPosition2DWindow(window, &Camera::m_Position, -1000.0f, 1000.0f, 0.5f, []() {});
    // };

    auto button_positionCamera = window->AddButton("Posicao", CRGBA(255, 255, 255));
    button_positionCamera->onClick = [window]() {
        menuVSL->AddVector2Window(window, &Camera::m_Position, -1000.0f, 1000.0f, 0.5f);
    };

    // auto button_sizeCamera = window->AddButton(91, 0, 0);
    // button_sizeCamera->onClick = [window]() {
    //     Menu::AddPosition2DWindow(window, &Camera::m_Size, -1000.0f, 1000.0f, 0.5f, []() {});
    // };

    auto button_sizeCamera = window->AddButton("Tamanho", CRGBA(255, 255, 255));
    button_sizeCamera->onClick = [window]() {
        menuVSL->AddVector2Window(window, &Camera::m_Size, -1000.0f, 1000.0f, 0.5f);
    };

    // auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    // button_close->onClick = []()
    // {
    //     Remove();
    //     ModConfig::SaveSettings();
    //     Create();
    // };

    auto button_close = window->AddButton("~r~Fechar");
    button_close->onClick = []()
    {
        Remove();
        ModConfig::SaveSettings();
        Create();
    };
}