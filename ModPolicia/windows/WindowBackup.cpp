#include "WindowBackup.h"

#include "../CleoFunctions.h"
#include "../Backup.h"

Window* WindowBackup::m_Window = NULL;

void WindowBackup::Create()
{
    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto button_normalBackup = window->AddButton(75);
    button_normalBackup->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio

        Backup::CallBackupCar(Backup::m_DataBackupVehicles[0]);
        Remove();
    };

    auto button_backupRocam = window->AddButton(76);
    button_backupRocam->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX78", 0, 0, 0, 3000, 1); //apoio
        Backup::CallBackupCar(Backup::m_DataBackupVehicles[1]);
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
        Backup::CallBackupCar(Backup::m_DataBackupVehicles[2]);
        Remove();
    };


    auto button_backupSf = window->AddButton(95);
    button_backupSf->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio

        Backup::CallBackupCar(Backup::m_DataBackupVehicles[3]);

        Remove();
    };

    auto button_backupLv = window->AddButton(96);
    button_backupLv->onClick = []()
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio

        Backup::CallBackupCar(Backup::m_DataBackupVehicles[4]);
        Remove();
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowBackup::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}