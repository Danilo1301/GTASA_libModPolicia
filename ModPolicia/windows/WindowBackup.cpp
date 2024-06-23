#include "WindowBackup.h"

#include "WindowCarMenu.h"

#include "CleoFunctions.h"
#include "Backup.h"
#include "Chase.h"
#include "Mod.h"
#include "SoundSystem.h"
#include "PoliceDepartment.h"

Window* WindowBackup::m_Window = NULL;
//Window* WindowBackup::m_BackupConfigWindow = NULL;
bool WindowBackup::m_CloseToBackupWindow = false;

void WindowBackup::Create()
{
    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto button_normalBackup = window->AddButton(75);
    button_normalBackup->onClick = []()
    {
        Remove();
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[0]);
    };

    auto button_backupRocam = window->AddButton(76);
    button_backupRocam->onClick = []()
    {
        Remove();
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX78", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[1]);
    };

    auto button_backupHeli = window->AddButton(87);
    button_backupHeli->onClick = []()
    {
        Remove();
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX88", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupHeli();
    };

    auto button_roadblocks = window->AddButton(122);
    button_roadblocks->onClick = []()
    {
        Remove();

        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX124", 0, 0, 0, 3000, 1); //apoio bloqueio

        SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/REQUEST_ROADBLOCK_", false);

        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        auto position = Mod::GetPedPositionWithOffset(playerActor, CVector(0, 100, 0));

        Chase::AddRoadBlocks(position);
    };

    auto button_spikestrips = window->AddButton(123);
    button_spikestrips->onClick = []()
    {
        Remove();

        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX125", 0, 0, 0, 3000, 1); //apoio espinhos

        SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/SPIKES_DEPLOYED_", false);

        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        auto position = Mod::GetPedPositionWithOffset(playerActor, CVector(0, 100, 0));

        Chase::AddSpikestrips(position);
    };

    auto button_backupFBI = window->AddButton(93);
    button_backupFBI->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX94", 0, 0, 0, 3000, 1); //apoio
        Remove();
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[2]);
    };

    auto button_backupSf = window->AddButton(95);
    button_backupSf->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio
        Remove();
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[3]);
    };

    auto button_backupLv = window->AddButton(96);
    button_backupLv->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio
        Remove();
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[4]);
    };

    auto button_backupRanger = window->AddButton(98);
    button_backupRanger->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX94", 0, 0, 0, 3000, 1); //apoio
        Remove();
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[5]);
    };

    auto button_backupSWAT = window->AddButton(138);
    button_backupSWAT->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX94", 0, 0, 0, 3000, 1); //apoio
        Remove();
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[7]);
    };

    auto button_config = window->AddButton(107);
    button_config->onClick = [window]()
    {
        CreateBackupConfig(window);
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowBackup::Remove()
{
    if(m_Window)
    {
        m_Window->RemoveThisWindow();
        m_Window = NULL;
    }

    /*
    if(m_BackupConfigWindow)
    {
        m_BackupConfigWindow->RemoveThisWindow();
        m_BackupConfigWindow = NULL;
    }
    */
}

void WindowBackup::CreateBackupConfig(Window* parent)
{
    auto window = Menu::AddWindow(6, parent);
    window->showPageControls = true;

    window->AddText(107);

    auto button_normalBackup = window->AddButton(75);
    button_normalBackup->onClick = [window]()
    {
        Remove();
        CreateBackupConfigForBackup(window, &Backup::m_DataBackupVehicles[0]);
    };

    auto button_backupRocam = window->AddButton(76);
    button_backupRocam->onClick = [window]()
    {
        Remove();
        CreateBackupConfigForBackup(window, &Backup::m_DataBackupVehicles[1]);
    };

    auto button_backupFBI = window->AddButton(93);
    button_backupFBI->onClick = [window]()
    {
        Remove();
        CreateBackupConfigForBackup(window, &Backup::m_DataBackupVehicles[2]);
    };


    auto button_backupSf = window->AddButton(95);
    button_backupSf->onClick = [window]()
    {
        Remove();
        CreateBackupConfigForBackup(window, &Backup::m_DataBackupVehicles[3]);
    };

    auto button_backupLv = window->AddButton(96);
    button_backupLv->onClick = [window]()
    {
        Remove();
        CreateBackupConfigForBackup(window, &Backup::m_DataBackupVehicles[4]);
    };

    auto button_backupRanger = window->AddButton(98);
    button_backupRanger->onClick = [window]()
    {
        Remove();
        CreateBackupConfigForBackup(window, &Backup::m_DataBackupVehicles[5]);
    };

    auto button_backupSWAT = window->AddButton(138);
    button_backupSWAT->onClick = [window]()
    {
        Remove();
        CreateBackupConfigForBackup(window, &Backup::m_DataBackupVehicles[7]);
    };

    /*
    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        
        if(m_CloseToBackupWindow) {
            Create();
            m_CloseToBackupWindow = false;
        } else {
            WindowCarMenu::Create(WindowCarMenu::m_Vehicle);
        }
    };
    */
}

int GetOptionByWeaponId(int weaponId)
{
    int i = 0;
    for(auto weapon : PoliceDepartment::m_Weapons)
    {
        if(weapon.weaponId == weaponId) return i;
        i++;
    }
    return 0;
}

void WindowBackup::CreateBackupConfigForBackup(Window* parent, BackupVehicle* backupVehicle)
{
    auto window = Menu::AddWindow(6, parent);
    window->showPageControls = true;

    window->AddIntRange(106, &backupVehicle->numPeds, 1, backupVehicle->maxPeds, 1);

    auto weaponOptions = window->AddOptions(108);
    weaponOptions->optionsValue = GetOptionByWeaponId(backupVehicle->weaponId);
    for(auto weapon : PoliceDepartment::m_Weapons)
    {
        weaponOptions->AddOption(weapon.gxtId, 0, 0);
    }
    weaponOptions->onValueChange = [weaponOptions, backupVehicle]() {
        backupVehicle->weaponId = PoliceDepartment::m_Weapons[weaponOptions->optionsValue].weaponId;
    };

    /*
    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        CreateBackupConfig();
    };
    */
}