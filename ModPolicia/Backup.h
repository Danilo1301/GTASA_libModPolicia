#pragma once

#include "pch.h"

#include "Vehicle.h"

enum BACKUP_TYPE {
	BACKUP_CHASE,
	BACKUP_CALLOUT
};

struct BackupVehicle {
    int vehicleModelId;
    int pedModelId;
    bool hasPassenger;
    int weaponId;
};

class Backup {
public:
    static std::vector<Vehicle*> m_BackupVehicles;
	static BACKUP_TYPE m_BackupType;
	static std::vector<BackupVehicle> m_DataBackupVehicles;

    static void Update(int dt);
    static void UpdateBackupVehiclesActionStatus(int dt);
    static void UpdateChaseBackup(int dt);
    static void UpdateCalloutBackup(int dt);

    static void CallBackupCar(BackupVehicle backupVehicle);
    static void CallBackupHeli();
};