#include "WindowBackup.h"

#include "../CleoFunctions.h"
#include "../Backup.h"

Window* WindowBackup::m_Window = NULL;
Window* WindowBackup::m_BackupConfigWindow = NULL;
bool WindowBackup::m_CloseToBackupWindow = false;

void WindowBackup::Create()
{
    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto button_normalBackup = window->AddButton(75);
    button_normalBackup->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[0]);
        Remove();
    };

    auto button_backupRocam = window->AddButton(76);
    button_backupRocam->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX78", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[1]);
        Remove();
    };

    auto button_backupHeli = window->AddButton(87);
    button_backupHeli->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX88", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupHeli();
        Remove();
    };

    auto button_backupFBI = window->AddButton(93);
    button_backupFBI->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX94", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[2]);
        Remove();
    };


    auto button_backupSf = window->AddButton(95);
    button_backupSf->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[3]);
        Remove();
    };

    auto button_backupLv = window->AddButton(96);
    button_backupLv->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[4]);
        Remove();
    };

    auto button_backupRanger = window->AddButton(98);
    button_backupRanger->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX94", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupCar(&Backup::m_DataBackupVehicles[5]);
        Remove();
    };

    auto button_config = window->AddButton(107);
    button_config->onClick = []()
    {
        Remove();
        m_CloseToBackupWindow = true;
        CreateBackupConfig();
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

    if(m_BackupConfigWindow)
    {
        m_BackupConfigWindow->RemoveThisWindow();
        m_BackupConfigWindow = NULL;
    }
}

void WindowBackup::CreateBackupConfig()
{
    auto window = m_BackupConfigWindow = Menu::AddWindow(6);
    window->showPageControls = true;

    window->AddText(107);

    auto button_normalBackup = window->AddButton(75);
    button_normalBackup->onClick = []()
    {
        Remove();
        CreateBackupConfigForBackup(&Backup::m_DataBackupVehicles[0]);
    };

    auto button_backupRocam = window->AddButton(76);
    button_backupRocam->onClick = []()
    {
        Remove();
        CreateBackupConfigForBackup(&Backup::m_DataBackupVehicles[1]);
    };

    auto button_backupFBI = window->AddButton(93);
    button_backupFBI->onClick = []()
    {
        Remove();
        CreateBackupConfigForBackup(&Backup::m_DataBackupVehicles[2]);
    };


    auto button_backupSf = window->AddButton(95);
    button_backupSf->onClick = []()
    {
        Remove();
        CreateBackupConfigForBackup(&Backup::m_DataBackupVehicles[3]);
    };

    auto button_backupLv = window->AddButton(96);
    button_backupLv->onClick = []()
    {
        Remove();
        CreateBackupConfigForBackup(&Backup::m_DataBackupVehicles[4]);
    };

    auto button_backupRanger = window->AddButton(98);
    button_backupRanger->onClick = []()
    {
        Remove();
        CreateBackupConfigForBackup(&Backup::m_DataBackupVehicles[5]);
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        
        if(m_CloseToBackupWindow) Create();
        m_CloseToBackupWindow = false;
    };
}

int GetOptionByWeaponId(int weaponId)
{
    int i = 0;
    for(auto id : Backup::m_DataBackupWeapons)
    {
        if(id == weaponId) return i;
        i++;
    }
    return 0;
}

void WindowBackup::CreateBackupConfigForBackup(BackupVehicle* backupVehicle)
{
    auto window = m_BackupConfigWindow = Menu::AddWindow(6);
    window->showPageControls = true;

    window->AddIntRange(106, &backupVehicle->numPeds, 1, backupVehicle->maxPeds, 1);

    auto weaponOptions = window->AddOptions(108);
    weaponOptions->optionsValue = GetOptionByWeaponId(backupVehicle->weaponId);
    for(auto weaponId : Backup::m_DataBackupWeapons)
    {
        weaponOptions->AddOption(105, weaponId, 0);
    }
    weaponOptions->onValueChange = [weaponOptions, backupVehicle]() {
        backupVehicle->weaponId = Backup::m_DataBackupWeapons[weaponOptions->optionsValue];
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        CreateBackupConfig();
    };
}