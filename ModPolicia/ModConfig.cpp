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
    for (auto& p : std::filesystem::recursive_directory_iterator(s))
        if (p.is_directory())
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

        for(auto patch : prevInfo->patches)
        {
            patch();
        }
        prevInfo->patches.clear();

        index++;
    }
}

//

bool ModConfig::EnableTestMenu = false;
bool ModConfig::CreateTestOptionsInRadioMenu = false;

void ModConfig::MakePaths()
{
    CreateFolder(GetConfigFolder());
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

void ModConfig::ConfigDeleteFile(std::string path)
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
    snprintf(path, sizeof(path), "%s/modPolicia", aml->GetConfigPath());
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
    generalSection->AddBool("enable_test_options_in_radio_menu", ModConfig::CreateTestOptionsInRadioMenu);
    generalSection->AddInt("time_between_callouts", Callouts::m_TimeBetweenCallouts);

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

    for(auto backupVehicle : Backup::m_DataBackupVehicles)
    {
        auto backupSection = file.AddSection("Backup_" + std::to_string(backupVehicle.vehicleModelId));

        backupSection->AddInt("num_peds", backupVehicle.numPeds);
        backupSection->AddInt("weapon_id", backupVehicle.weaponId);
    }

    file.Save(settingsFileDir);
    file.Destroy();
}

void ModConfig::Load()
{
    MakePaths();

    LoadSettings();
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
    }

    auto generalSections = file.GetSections("General");
    if (generalSections.size() > 0)
    {
        auto generalSection = generalSections[0];

        generalSection->GetBool("enable_test_options_in_radio_menu", &ModConfig::CreateTestOptionsInRadioMenu);
        generalSection->GetInt("time_between_callouts", &Callouts::m_TimeBetweenCallouts);
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

    //

    for(auto& backupVehicle : Backup::m_DataBackupVehicles)
    {
        std::string backupSectionName = "Backup_" + std::to_string(backupVehicle.vehicleModelId);

        auto sections = file.GetSections(backupSectionName);
        if (sections.size() > 0)
        {
            auto section = sections[0];

            section->GetInt("num_peds", &backupVehicle.numPeds);
            section->GetInt("weapon_id", &backupVehicle.weaponId);

            Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Loaded " << backupSectionName << std::endl;
        }
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Success reading settings.ini" << std::endl;
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

    VersionControl::SetVersion(ReadVersionFile(), Mod::m_Version);
}

void ModConfig::ProcessVersionChanges_PreConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [PRE] Updating from " << prevVersion << " to " << currentVersion << std::endl;

    //if (prevVersion == currentVersion) return;

    //-------------

    /*
    VersionControl::AddPatch("1.0.1", [] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Patch 1" << std::endl;
    });
    */

    VersionControl::ApplyPatches();

    //-------------

}

void ModConfig::ProcessVersionChanges_PostConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [POST] Updating from " << prevVersion << " to " << currentVersion << std::endl;
    
    //if (prevVersion == currentVersion) return;

    //-------------

    VersionControl::AddPatch("1.0.2", [] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Patch 1.0.2 POST" << std::endl;

        Ped::CHANCE_PED_BEEING_DRUG_DEALER = 0.2f;
        Ped::CHANCE_PED_CONSUME_DRUGS = 0.4f;
    });

    VersionControl::ApplyPatches();

    //-------------

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Saving version file" << std::endl;

    std::string path = GetConfigFolder() + "/version";

    std::fstream file;
    file.open(path, std::fstream::out);
    file << currentVersion;
    file.close();
}