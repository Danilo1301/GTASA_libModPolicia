#pragma once

#include "pch.h"

#include "Vehicle.h"
#include "Ped.h"
#include "audiosystem.h"

enum BACKUP_TYPE {
	BACKUP_CHASE,
	BACKUP_CALLOUT
};

/*
int vehicleModelId;
int pedModelId;
int numPeds;
int maxPeds;
int weaponId;
std::string soundRequest;
std::string soundIncoming;
*/
struct BackupVehicle {
    std::string name;
    int vehicleModelId;
    int pedModelId;
    int numPeds;
    int maxPeds;
    int weaponId;
    std::string soundRequest;
    std::string soundIncoming;
};

class Backup {
public:
    static std::vector<Vehicle*> m_BackupVehicles;
    static std::vector<Ped*> m_BackupPeds;
	static BACKUP_TYPE m_BackupType;
	static std::vector<BackupVehicle> m_DataBackupVehicles;
	static std::vector<int> m_DataBackupWeapons;

    static CAudioStream* m_RequestBackupAudio;
    static bool m_WaitingToRespondDispatch;
    static std::string m_IncomingBackupSoundName;

    static void Update(int dt);
    static void UpdateBackupPedsAndCars(int dt);
    static void UpdateChaseBackup(int dt);
    static void UpdateCalloutBackup(int dt);

    static void SpawnBackupCar(BackupVehicle* backupVehicle, CVector position);
    static void CallBackupCar(BackupVehicle* backupVehicle);
    static void CallBackupHeli();

    static void PlayRequestBackupAudio(BackupVehicle* backupVehicle);

    static int FindClosestCop(CVector position, float radius, bool includePlayer);
    static bool IsPedACop(Ped* ped);

    static Vehicle* GetCarThatCopBelongs(Ped* cop);
};