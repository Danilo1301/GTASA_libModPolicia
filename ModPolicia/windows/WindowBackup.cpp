#include "WindowBackup.h"

#include "WindowCarMenu.h"

#include "CleoFunctions.h"
#include "Backup.h"
#include "Chase.h"
#include "Mod.h"
#include "SoundSystem.h"
#include "PoliceDepartment.h"
#include "systems/Names.h"

IWindow* WindowBackup::m_Window = NULL;

extern IMenuVSL* menuVSL;

void WindowBackup::Create()
{
}

void WindowBackup::Remove()
{
    if(m_Window)
    {
        m_Window->SetToBeRemoved();
        m_Window = NULL;
    }
}

void WindowBackup::CreateBackupConfig(IWindow* parent)
{
    auto window = menuVSL->AddWindow();
    window->m_Parent = parent;
    window->m_ShowBackButton = true;
    window->m_Title = GetLanguageLine("config_backup");

    for(int i = 0; i < Backup::m_DataBackupVehicles.size(); i++)
    {
        auto data = &Backup::m_DataBackupVehicles[i];

        std::string backupName = GetLanguageLine(data->name);
        std::string buttonText = GetLanguageLine("config_backup_item", backupName.c_str());

        auto button = window->AddButton(buttonText);
        button->onClick = [window, data]()
        {
            CreateBackupConfigForBackup(window, data);
        };
    }
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

void WindowBackup::CreateBackupConfigForBackup(IWindow* parent, BackupVehicle* backupVehicle)
{
    auto window = menuVSL->AddWindow();
    window->m_Parent = parent;
    window->m_ShowBackButton = true;
    window->m_Title = GetLanguageLine("config_backup");
    
    window->AddIntRange(GetLanguageLine("num_of_peds"), &backupVehicle->numPeds, 1, backupVehicle->maxPeds, 1);

    auto weaponOptions = window->AddOptions(GetLanguageLine("backup_weapon"));
    for(auto weapon : PoliceDepartment::m_Weapons)
    {
        weaponOptions->AddOption(weapon.weaponId, Names::GetWeaponName(weapon.weaponId));
    }
    weaponOptions->SetCurrentOption(backupVehicle->weaponId);
    weaponOptions->onValueChange = [weaponOptions, backupVehicle]() {
        backupVehicle->weaponId = weaponOptions->GetCurrentOption().value;
    };
}