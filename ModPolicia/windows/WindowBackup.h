#pragma once

#include "../pch.h"

#include "../menu/Menu.h"

#include "../Backup.h"

class WindowBackup {
public:
    static Window* m_Window;
    //static Window* m_BackupConfigWindow;
	static bool m_CloseToBackupWindow;
    
	static void Create();
	static void Remove();

	static void CreateBackupConfig(Window* parent);
	static void CreateBackupConfigForBackup(Window* parent, BackupVehicle* backupVehicle);
};