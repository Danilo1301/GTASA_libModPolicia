#pragma once

#include "../pch.h"

#include "../menu/Menu.h"

#include "../Backup.h"

#include "menu/IMenuVSL.h"

class WindowBackup {
public:
    static IWindow* m_Window;
    
	static void Create();
	static void Remove();

	static void CreateBackupConfig(IWindow* parent);
	static void CreateBackupConfigForBackup(IWindow* parent, BackupVehicle* backupVehicle);
};