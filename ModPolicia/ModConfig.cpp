#include "ModConfig.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>
//#include <dirent.h>
//#include "dlfcn.h"

#include "mod/amlmod.h"
#include "mod/logger.h"
#include "mod/config.h"

#include "iniconfig/INIFile.h"

#include "menu/Window.h"

#include "Mod.h"
#include "Log.h"
#include "Ped.h"
#include "Vehicle.h"
#include "Callouts.h"
#include "Backup.h"
#include "Trunk.h"
#include "PoliceDepartment.h"
#include "Stats.h"
#include "Chase.h"
#include "Pullover.h"
#include "Ambulance.h"

#include "systems/Camera.h"

#include "windows/WindowRadio.h"

bool isDirExist(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

std::vector<std::string> get_directories_name(const std::string& s)
{
    std::vector<std::string> r;
    for (auto& p : std::filesystem::directory_iterator(s))
        if (p.is_directory())
            r.push_back(p.path().filename().string());
    return r;
}

std::vector<std::string> get_files_name(const std::string& s)
{
    std::vector<std::string> r;
    for (auto& p : std::filesystem::directory_iterator(s))
        if (!p.is_directory())
            r.push_back(p.path().filename().string());
    return r;
}

//

std::vector<VersionInfo*> VersionControl::m_Versions;
std::string VersionControl::m_PrevVersion = "";
std::string VersionControl::m_CurrentVersion = "";

void VersionControl::SetVersion(std::string prevVersion, std::string currentVersion)
{
    m_PrevVersion = prevVersion;
    m_CurrentVersion = currentVersion;
}

void VersionControl::AddVersion(std::string version)
{
    VersionInfo* info = new VersionInfo();
    info->version = version;

    m_Versions.push_back(info);
}

VersionInfo* VersionControl::GetVersionInfo(std::string version)
{
    for(auto info : m_Versions)
    {
        if(info->version == version) return info;
    }
    return NULL;
}

void VersionControl::AddPatch(std::string version, std::function<void()> patch)
{
    VersionInfo* info = GetVersionInfo(version);
    info->patches.push_back(patch);
}

void VersionControl::AddPatch(std::function<void()> patch)
{
    for(auto info : m_Versions)
    {
        info->patches.push_back(patch);
    }
}

void VersionControl::ApplyPatches()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: ApplyPatches" << std::endl;

    auto prevVersion = m_PrevVersion;
    VersionInfo* prevInfo = NULL;

    if(prevVersion == m_CurrentVersion)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: Same version, no need to apply patches" << std::endl;
        return;
    }

    if(prevVersion == "unknown")
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: Version is unknown, so its the first time run, no need to apply patches" << std::endl;
        return;
    }

    int index = 0;
    for(auto info : m_Versions)
    {
        if(info->version == prevVersion)
        {
            prevInfo = info;
            break;
        }

        index++;
    }

    while (index < m_Versions.size() - 1)
    {
        prevInfo = m_Versions[index];

        Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: Processing index " << index << ", version " << prevInfo->version << std::endl;

        if(prevInfo->patches.size() > 0)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: Applying " << prevInfo->patches.size() << " patches..." << std::endl;
        }

        for(auto patch : prevInfo->patches)
        {
            patch();
        }
        prevInfo->patches.clear();

        index++;
    }
}

//

std::string ModConfig::m_ConfigMainFolderName = "modPolicia";
bool ModConfig::EnableTestMenu = false;
bool ModConfig::CreateTestOptionsInRadioMenu = false;
bool ModConfig::EnableModWhenGameStarts = false;
bool ModConfig::StartGameWithRadio = false;

void ModConfig::MakePaths()
{
    CreateFolder(GetConfigFolder());
    CreateFolder(GetConfigFolder() + "/data");
    CreateFolder(GetConfigFolder() + "/data/bases");
    CreateFolder(GetConfigFolder() + "/assets");
    CreateFolder(GetConfigFolder() + "/audios");
}

bool ModConfig::DirExists(std::string path)
{
    return isDirExist(path);
}

bool ModConfig::FileExists(std::string path)
{
    return file_exists(path);
}

std::vector<std::string> ModConfig::GetDirectoriesName(std::string path)
{
    return get_directories_name(path);
}

std::vector<std::string> ModConfig::GetFilesName(std::string path)
{
    return get_files_name(path);
}

void ModConfig::ConfigDelete(std::string path)
{
    try {
        if (std::filesystem::remove(path))
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: file " << path << " deleted" << std::endl;
        else
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: file " << path << " not found" << std::endl;
    }
    catch(const std::filesystem::filesystem_error& err) {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: delete file: filesystem error: " << err.what() << std::endl;
    }
}

std::string ModConfig::GetConfigFolder()
{
    char path[0xFF];
    snprintf(path, sizeof(path), "%s/%s", aml->GetConfigPath(), m_ConfigMainFolderName.c_str());
    return path;
}

void ModConfig::CreateFolder(std::string path)
{
    if (DirExists(path)) return;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: CreateFolder " << path << std::endl;

    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void ModConfig::WriteToFile(std::string path, Json::Value value) {
	Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: WriteToFile " << path << std::endl;

	Json::StyledWriter writer;
	std::string strJson = writer.write(value);

	std::ofstream file(path);
	file << strJson;
	file.close();
}

Json::Value ModConfig::ReadFile(std::string path) {
	Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: ReadFile " << path << std::endl;

	std::ifstream file(path);

	Json::Value value;
	Json::Reader reader;

	if (!reader.parse(file, value, true)) {
		//MessageBox(HWND_DESKTOP, std::string("Error loading " + path).c_str(), "GiroflexVSL", MB_ICONERROR);
	}

	return value;
}

void ModConfig::Save()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Save" << std::endl;

    MakePaths();

    SaveSettings();
}

void ModConfig::SaveSettings()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: SaveSettings (settings.ini)" << std::endl;

    auto settingsFileDir = GetConfigFolder() + "/settings.ini";

    INIFile file;
    
    auto generalSection = file.AddSection("General");
    generalSection->AddLine("");
    generalSection->AddLine("; Enables the test options in the radio menu");
    generalSection->AddLine("; 0 = disabled | 1 = enabled");
    generalSection->AddIntFromBool("enable_test_options_in_radio_menu", ModConfig::CreateTestOptionsInRadioMenu);

    generalSection->AddLine("");
    generalSection->AddLine("; Starts the game with mod enabled");
    generalSection->AddLine("; 0 = starts disabled | 1 = starts enabled");
    generalSection->AddIntFromBool("enable_mod_when_game_starts", ModConfig::EnableModWhenGameStarts);
    generalSection->AddLine("");

    generalSection->AddInt("time_between_callouts", Callouts::m_TimeBetweenCallouts);
    generalSection->AddInt("money_reward", PoliceDepartment::m_MoneyReward);
    generalSection->AddIntFromBool("enable_deep_log", Log::deepLogEnabled);
    generalSection->AddIntFromBool("pullover_play_animation", Pullover::PULLOVER_PLAY_ANIMATION);
    generalSection->AddIntFromBool("start_game_with_radio", ModConfig::StartGameWithRadio);
    generalSection->AddIntFromBool("transparent_radio_buttons", WindowRadio::m_TransparentButtons);

    generalSection->AddLine("");
    generalSection->AddLine("; Distance that Ambulance and IML will spawn");
    generalSection->AddFloat("spawn_emergency_vehicles_distance", Ambulance::SPAWN_EMERGENCY_VEHICLES_DISTANCE);
    generalSection->AddLine("");

    generalSection->AddFloat("chase_vehicle_max_speed", Chase::CHASE_VEHICLE_MAX_SPEED);

    //

    auto chancesSection = file.AddSection("Chances");
    chancesSection->AddFloat("CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME", Ped::CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME);
    chancesSection->AddFloat("CHANCE_PED_BEEING_DRUG_DEALER", Ped::CHANCE_PED_BEEING_DRUG_DEALER);
    chancesSection->AddFloat("CHANCE_PED_CONSUME_DRUGS", Ped::CHANCE_PED_CONSUME_DRUGS);
    chancesSection->AddFloat("CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE", Ped::CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE);
    chancesSection->AddFloat("CHANCE_PED_BEEING_WANTED", Ped::CHANCE_PED_BEEING_WANTED);

    chancesSection->AddFloat("CHANCE_VEHICLE_BEEING_STOLEN", Vehicle::CHANCE_VEHICLE_BEEING_STOLEN);
    chancesSection->AddFloat("CHANCE_VEHICLE_HAVING_GUNS", Vehicle::CHANCE_VEHICLE_HAVING_GUNS);  

    //

    auto windowSection = file.AddSection("Window");
    windowSection->AddFloat("position_x", Window::m_DefaultWindowPosition.x);
    windowSection->AddFloat("position_y", Window::m_DefaultWindowPosition.y);

    //

    auto cameraSection = file.AddSection("Camera");
    cameraSection->AddCVector2D("position", Camera::m_Position);
    cameraSection->AddCVector2D("size", Camera::m_Size);

    //

    file.Save(settingsFileDir);
    file.Destroy();

    SaveDataSettings();
}

void ModConfig::SaveDataSettings()
{
    auto dataDir = GetConfigFolder() + "/data/";

    //backupFile

    INIFile backupFile;

    for(auto backupVehicle : Backup::m_DataBackupVehicles)
    {
        auto backupSection = backupFile.AddSection("Backup_" + std::to_string(backupVehicle.vehicleModelId));

        backupSection->AddInt("num_peds", backupVehicle.numPeds);
        backupSection->AddInt("weapon_id", backupVehicle.weaponId);
    }

    backupFile.Save(dataDir + "backup.ini");
    backupFile.Destroy();

    //trunk

    INIFile trunkFile;

    for(auto pair : Trunk::m_TrunkModels)
    {
        auto modelId = pair.first;
        auto trunkModelData = &pair.second;

        auto backupSection = trunkFile.AddSection("Trunk_" + std::to_string(modelId));

        backupSection->AddCVector("position1", trunkModelData->trunkPositions[0]);
        backupSection->AddCVector("position2", trunkModelData->trunkPositions[1]);
    }

    trunkFile.Save(dataDir + "trunk.ini");
    trunkFile.Destroy();

    //pickups

    INIFile pickupsFile;

    auto pickupsSection = pickupsFile.AddSection("PICKUPS");
    pickupsSection->AddInt("pickup_equipment_model_id", PoliceDepartment::m_PickupEquipmentId);
    pickupsSection->AddInt("pickup_duty_id", PoliceDepartment::m_PickupDutyId);
    pickupsSection->AddInt("pickup_partner_model_id", PoliceDepartment::m_PickupPartnerId);

    pickupsFile.Save(dataDir + "pickups.ini");
    pickupsFile.Destroy();

    //barriers

    INIFile barriersFile;
    
    auto barriersSection = barriersFile.AddSection("BARRIERS");
    barriersSection->AddInt("barrier_roadblock_ped_id", Chase::m_BarrierModels[0].pedModelId);
    barriersSection->AddInt("barrier_roadblock_vehicle_id", Chase::m_BarrierModels[0].vehicleModelId);

    barriersSection->AddInt("barrier_spikes_ped_id", Chase::m_BarrierModels[1].pedModelId);
    barriersSection->AddInt("barrier_spikes_vehicle_id", Chase::m_BarrierModels[1].vehicleModelId);

    barriersFile.Save(dataDir + "barriers.ini");
    barriersFile.Destroy();
}

void ModConfig::SaveStats()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: SaveStats (data/stats.ini)" << std::endl;

    auto dir = GetConfigFolder() + "/data/stats.ini";

    INIFile file;
    
    auto section = file.AddSection("STATS");
    section->AddInt("times_opened_game", Stats::TimesOpenedGame);
    section->AddInt("time_played", Stats::TimePlayed);

    file.Save(dir);
    file.Destroy();
}

void ModConfig::Load()
{
    MakePaths();

    LoadSettings();
    LoadStats();

    PoliceDepartment::LoadBases();
}

void ModConfig::LoadSettings()
{
    auto settingsFileDir = GetConfigFolder() + "/settings.ini";

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: LoadSettings (settings.ini)" << std::endl;

    if(ModConfig::FileExists(settingsFileDir))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Settings file exists" << std::endl;
    } else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Settings file doesn't exist" << std::endl;
    }

    INIFile file;
    if (!file.Read(settingsFileDir))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading settings.ini (Not found)" << std::endl;
        return;
    }

    for(auto section : file.sections)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Section: " << section->key << " has " << section->values.size() << " values" << std::endl;

        /*
        for(auto value : section->values)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << value.first << "|" << value.second << std::endl;
        }
        */
    }

    auto generalSections = file.GetSections("General");
    if (generalSections.size() > 0)
    {
        auto generalSection = generalSections[0];

        generalSection->GetBoolFromInt("enable_test_options_in_radio_menu", &ModConfig::CreateTestOptionsInRadioMenu);
        generalSection->GetBoolFromInt("enable_mod_when_game_starts", &ModConfig::EnableModWhenGameStarts);        
        generalSection->GetInt("time_between_callouts", &Callouts::m_TimeBetweenCallouts);
        generalSection->GetInt("money_reward", &PoliceDepartment::m_MoneyReward);
        generalSection->GetBoolFromInt("enable_deep_log", &Log::deepLogEnabled);
        generalSection->GetBoolFromInt("pullover_play_animation", &Pullover::PULLOVER_PLAY_ANIMATION);  
        generalSection->GetBoolFromInt("start_game_with_radio", &ModConfig::StartGameWithRadio);  
        generalSection->GetBoolFromInt("transparent_radio_buttons", &WindowRadio::m_TransparentButtons);
        generalSection->GetFloat("spawn_emergency_vehicles_distance", &Ambulance::SPAWN_EMERGENCY_VEHICLES_DISTANCE);
        generalSection->GetFloat("chase_vehicle_max_speed", &Chase::CHASE_VEHICLE_MAX_SPEED);
    }

    //

    auto chancesSections = file.GetSections("Chances");
    if (chancesSections.size() > 0)
    {
        auto chancesSection = chancesSections[0];

        chancesSection->GetFloat("CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME", &Ped::CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME);
        chancesSection->GetFloat("CHANCE_PED_BEEING_DRUG_DEALER", &Ped::CHANCE_PED_BEEING_DRUG_DEALER);
        chancesSection->GetFloat("CHANCE_PED_CONSUME_DRUGS", &Ped::CHANCE_PED_CONSUME_DRUGS);
        chancesSection->GetFloat("CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE", &Ped::CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE);
        chancesSection->GetFloat("CHANCE_PED_BEEING_WANTED", &Ped::CHANCE_PED_BEEING_WANTED);

        chancesSection->GetFloat("CHANCE_VEHICLE_BEEING_STOLEN", &Vehicle::CHANCE_VEHICLE_BEEING_STOLEN);
        chancesSection->GetFloat("CHANCE_VEHICLE_HAVING_GUNS", &Vehicle::CHANCE_VEHICLE_HAVING_GUNS);
    }

    //

    auto windowSections = file.GetSections("Window");
    if (windowSections.size() > 0)
    {
        auto windowSection = windowSections[0];

        windowSection->GetFloat("position_x", &Window::m_DefaultWindowPosition.x);
        windowSection->GetFloat("position_y", &Window::m_DefaultWindowPosition.y);
    }

    auto cameraSections = file.GetSections("Camera");
    if (cameraSections.size() > 0)
    {
        auto cameraSection = cameraSections[0];

        cameraSection->GetCVector2D("position", &Camera::m_Position);
        cameraSection->GetCVector2D("size", &Camera::m_Size);
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Success reading settings.ini" << std::endl;

    LoadDataSettings();
}

void ModConfig::LoadDataSettings()
{
    auto dataDir = GetConfigFolder() + "/data/";

    INIFile backupFile;
    if (backupFile.Read(dataDir + "backup.ini"))
    {
        for(auto& backupVehicle : Backup::m_DataBackupVehicles)
        {
            std::string backupSectionName = "Backup_" + std::to_string(backupVehicle.vehicleModelId);

            auto sections = backupFile.GetSections(backupSectionName);
            if (sections.size() > 0)
            {
                auto section = sections[0];

                section->GetInt("num_peds", &backupVehicle.numPeds);
                section->GetInt("weapon_id", &backupVehicle.weaponId);

                Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Loaded " << backupSectionName << std::endl;
            }
        }
    } else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading backup.ini (Not found)" << std::endl;
    }

    INIFile trunkFile;
    if (trunkFile.Read(dataDir + "trunk.ini"))
    {   
        for(auto pair : Trunk::m_TrunkModels)
        {
            auto modelId = pair.first;
            auto trunkModelData = &Trunk::m_TrunkModels[modelId];

            std::string sectionName = "Trunk_" + std::to_string(modelId);

            auto sections = trunkFile.GetSections(sectionName);
            if (sections.size() > 0)
            {
                auto section = sections[0];

                section->GetCVector("position1", &trunkModelData->trunkPositions[0]);
                section->GetCVector("position2", &trunkModelData->trunkPositions[1]);

                Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Loaded " << sectionName << std::endl;
            }
        }
    } else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading trunk.ini (Not found)" << std::endl;
    }

    INIFile pickupFile;
    if (pickupFile.Read(dataDir + "pickups.ini"))
    {   
        auto sections = pickupFile.GetSections("PICKUPS");
        auto section = sections[0];

        section->GetInt("pickup_equipment_model_id", &PoliceDepartment::m_PickupEquipmentId);
        section->GetInt("pickup_duty_id", &PoliceDepartment::m_PickupDutyId);
        section->GetInt("pickup_partner_model_id", &PoliceDepartment::m_PickupPartnerId);
    } else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading pickups.ini (Not found)" << std::endl;
    }

    INIFile barriersFile;
    if (barriersFile.Read(dataDir + "barriers.ini"))
    {   
        auto sections = barriersFile.GetSections("BARRIERS");
        auto section = sections[0];

        section->GetInt("barrier_roadblock_ped_id", &Chase::m_BarrierModels[0].pedModelId);
        section->GetInt("barrier_roadblock_vehicle_id", &Chase::m_BarrierModels[0].vehicleModelId);

        section->GetInt("barrier_spikes_ped_id", &Chase::m_BarrierModels[1].pedModelId);
        section->GetInt("barrier_spikes_vehicle_id", &Chase::m_BarrierModels[1].vehicleModelId);
    } else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading barriers.ini (Not found)" << std::endl;
    }
}

void ModConfig::LoadStats()
{
    auto dir = GetConfigFolder() + "/data/stats.ini";

    INIFile file;
    if (!file.Read(dir))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading stats.ini (Not found)" << std::endl;
        return;
    }

    auto sections = file.GetSections("STATS");
    if (sections.size() > 0)
    {
        auto section = sections[0];

        section->GetInt("times_opened_game", &Stats::TimesOpenedGame);
        section->GetInt("time_played", &Stats::TimePlayed);
    }
}

std::string ModConfig::ReadVersionFile()
{
    std::string prevVersion = "unknown";
    std::string path = GetConfigFolder() + "/version";

    std::ifstream file;
    file.open(path);
    if (file.good())
    {
        getline(file, prevVersion);
    }
    file.close();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: ReadVersionFile, version: " << prevVersion << std::endl;

    return prevVersion;
}

void ModConfig::DefineVersions()
{
    VersionControl::AddVersion("0.1.0");
    VersionControl::AddVersion("0.2.0");
    VersionControl::AddVersion("0.3.0");
    VersionControl::AddVersion("0.3.1");
    VersionControl::AddVersion("0.4.0");
    VersionControl::AddVersion("0.4.1");
    VersionControl::AddVersion("0.5.0");
    VersionControl::AddVersion("0.6.0");
    VersionControl::AddVersion("0.7.0");
    VersionControl::AddVersion("1.0.0");
    VersionControl::AddVersion("1.0.1");
    VersionControl::AddVersion("1.0.2");
    VersionControl::AddVersion("1.1.0");
    VersionControl::AddVersion("1.2.0");
    VersionControl::AddVersion("1.3.0");
    VersionControl::AddVersion("1.4.0");
    VersionControl::AddVersion("1.4.1");
    VersionControl::AddVersion("1.5.0");
    VersionControl::AddVersion("1.6.0");
    VersionControl::AddVersion("1.6.1");
    VersionControl::AddVersion("1.6.2");
    VersionControl::AddVersion("1.6.3");
    VersionControl::AddVersion("1.6.4");
    VersionControl::AddVersion("1.6.5");

    VersionControl::SetVersion(ReadVersionFile(), Mod::m_Version);
}

void ModConfig::ProcessVersionChanges_PreConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [PRE] Updating from " << prevVersion << " to " << currentVersion << std::endl;


    VersionControl::AddPatch( [] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Patch (Audios) PRE" << std::endl;

        std::vector<std::string> oldAudios = {
            "HELI_APPROACHING_DISPATCH_1.wav",
            "HELI_APPROACHING_DISPATCH_2.wav",
            "HELI_NO_VISUAL_DISPATCH_1.wav",
            "HELI_NO_VISUAL_DISPATCH_2.wav",
            "REQUEST_BACKUP_BIKE_1.wav",
            "REQUEST_BACKUP_BIKE_2.wav",
            "REQUEST_BACKUP_FBI_1.wav",
            "REQUEST_BACKUP_FBI_2.wav",
            "REQUEST_BACKUP_HELI_1.wav",
            "REQUEST_BACKUP_HELI_2.wav",
            "REQUEST_BACKUP_LS_1.wav",
            "REQUEST_BACKUP_LS_2.wav",
            "REQUEST_BACKUP_LV_1.wav",
            "REQUEST_BACKUP_LV_2.wav",
            "REQUEST_BACKUP_RANGER_1.wav",
            "REQUEST_BACKUP_RANGER_2.wav",
            "REQUEST_BACKUP_SF_1.wav",
            "REQUEST_BACKUP_SF_2.wav",
            "REQUEST_BACKUP_SWAT_1.wav",
            "REQUEST_BACKUP_SWAT_2.wav",
            "UNIT_RESPONDING_DISPATCH_1.wav",
            "UNIT_RESPONDING_DISPATCH_2.wav",
            "UNIT_RESPONDING_DISPATCH_3.wav",
            "UNIT_RESPONDING_DISPATCH_4.wav",
            "SPIKES_DEPLOYED_1.wav",
            "SPIKES_DEPLOYED_2.wav",
            "SPIKES_DEPLOYED_3.wav",
            "SPIKES_DEPLOYED_4.wav",
            "SPIKES_DEPLOYED_5.wav",
            "REQUEST_ROADBLOCK_1.wav",
            "ASK_STOP_PEDESTRIAN.wav",
            "ASK_STOP_VEHICLE.wav",
            "ACCEPT_CALLOUT.wav",
            "PULLOVER_FREE_PED.wav",

            "ASK_FOR_DRIVERS_LICENSE.wav", //edited
            "ASK_FOR_ID.wav", //edited
            "CHECK_ID.wav", //edited
            "CHECK_VEHICLE_PLATE.wav", //editado
            "ID_OK.wav", //edited
            "ID_WITH_ARREST_WARRANT.wav", //edited
            "REQUEST_AMBULANCE.wav", //edited
            "REQUEST_CAR_TO_TRANSPORT_SUSPECT.wav", //edited
            "REQUEST_IML.wav", //edited
            "REQUEST_TOW_TRUCK.wav", //edited
            "VEHICLE_PLATE_OK.wav", //edited
            "VEHICLE_PLATE_STOLEN.wav" //edited
        };
        
        auto audiosPath = ModConfig::GetConfigFolder() + "/audios/";
        auto unusedPath = audiosPath + "/unused_audios/";

        CreateFolder(unusedPath);

        for(auto wavName : oldAudios)
        {
            auto path = audiosPath + "/voices/" + wavName;

            if(!FileExists(path)) continue;

            auto newPath = unusedPath + wavName;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Moving to unused: " << wavName << std::endl;
            try {
                std::filesystem::rename(path, newPath);
            } catch (std::filesystem::filesystem_error& e) {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Could not move. Error: " << e.what() << std::endl;
            }
        }

        auto files = get_files_name(unusedPath);

        Log::Level(LOG_LEVEL::LOG_BOTH) << files.size() << " files in /unused_audios/" << std::endl;

        if(files.size() == 0)
        {
            ModConfig::ConfigDelete(unusedPath);
        }
    });



    VersionControl::ApplyPatches();
}

void ModConfig::ProcessVersionChanges_PostConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [POST] Updating from " << prevVersion << " to " << currentVersion << std::endl;
    
    //patches

    VersionControl::AddPatch("1.0.2", [] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Patch 1.0.2 POST" << std::endl;

        Ped::CHANCE_PED_BEEING_DRUG_DEALER = 0.2f;
        Ped::CHANCE_PED_CONSUME_DRUGS = 0.4f;
    });

    VersionControl::AddPatch("1.4.1", [] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Patch 1.4.1 POST" << std::endl;

        Stats::TimePlayed = 0;
    });

    VersionControl::AddPatch("1.6.0", [] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Patch 1.6.0 POST" << std::endl;

        Ped::CHANCE_PED_BEEING_DRUG_DEALER = 0.1f;
        Ped::CHANCE_PED_CONSUME_DRUGS = 0.2f;
    });

    VersionControl::ApplyPatches();

    //

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Saving version file" << std::endl;

    std::string path = GetConfigFolder() + "/version";

    std::fstream file;
    file.open(path, std::fstream::out);
    file << currentVersion;
    file.close();
}