#include "ModConfig.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include "dlfcn.h"

#include "mod/amlmod.h"
#include "mod/logger.h"
#include "mod/config.h"

#include "iniconfig/INIFile.h"

#include "Mod.h"
#include "Log.h"
#include "Ped.h"
#include "Vehicle.h"
#include "Callouts.h"

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

bool ModConfig::EnableTestMenu = false;

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
    generalSection->AddBool("enable_test_menu", ModConfig::EnableTestMenu);
    generalSection->AddInt("time_between_callouts", Callouts::m_TimeBetweenCallouts);

    auto chancesSection = file.AddSection("Chances");
    chancesSection->AddFloat("CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME", Ped::CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME);
    chancesSection->AddFloat("CHANCE_PED_BEEING_DRUG_DEALER", Ped::CHANCE_PED_BEEING_DRUG_DEALER);
    chancesSection->AddFloat("CHANCE_PED_CONSUME_DRUGS", Ped::CHANCE_PED_CONSUME_DRUGS);
    chancesSection->AddFloat("CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE", Ped::CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE);
    chancesSection->AddFloat("CHANCE_PED_BEEING_WANTED", Ped::CHANCE_PED_BEEING_WANTED);

    chancesSection->AddFloat("CHANCE_VEHICLE_BEEING_STOLEN", Vehicle::CHANCE_VEHICLE_BEEING_STOLEN);
    chancesSection->AddFloat("CHANCE_VEHICLE_HAVING_GUNS", Vehicle::CHANCE_VEHICLE_HAVING_GUNS);  

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

    INIFile file;
    if (!file.Read(settingsFileDir))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading settings.ini (Not found)" << std::endl;
        return;
    }

   
    auto generalSections = file.GetSections("General");
    if (generalSections.size() > 0)
    {
        auto generalSection = generalSections[0];

        generalSection->GetBoolDefault("enable_test_menu", &ModConfig::EnableTestMenu);
        generalSection->GetIntDefault("time_between_callouts", &Callouts::m_TimeBetweenCallouts);
    }

    auto chancesSections = file.GetSections("Chances");
    if (chancesSections.size() > 0)
    {
        auto chancesSection = chancesSections[0];

        chancesSection->GetFloatDefault("CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME", &Ped::CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME);
        chancesSection->GetFloatDefault("CHANCE_PED_BEEING_DRUG_DEALER", &Ped::CHANCE_PED_BEEING_DRUG_DEALER);
        chancesSection->GetFloatDefault("CHANCE_PED_CONSUME_DRUGS", &Ped::CHANCE_PED_CONSUME_DRUGS);
        chancesSection->GetFloatDefault("CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE", &Ped::CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE);
        chancesSection->GetFloatDefault("CHANCE_PED_BEEING_WANTED", &Ped::CHANCE_PED_BEEING_WANTED);

        chancesSection->GetFloatDefault("CHANCE_VEHICLE_BEEING_STOLEN", &Vehicle::CHANCE_VEHICLE_BEEING_STOLEN);
        chancesSection->GetFloatDefault("CHANCE_VEHICLE_HAVING_GUNS", &Vehicle::CHANCE_VEHICLE_HAVING_GUNS);

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

    return prevVersion;
}

void ModConfig::ProcessVersionChanges_PreConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [PRE] Updating from " << prevVersion << " to " << currentVersion << std::endl;

    if (prevVersion == currentVersion) return;

    //-------------

    /*
    if (prevVersion == "2.0.0-example")
    {
        prevVersion = "2.0.1";
    }
    */

    //-------------
}

void ModConfig::ProcessVersionChanges_PostConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [POST] Updating from " << prevVersion << " to " << currentVersion << std::endl;
    
    if (prevVersion == currentVersion) return;

    //-------------

    /*
    if (prevVersion == "2.0.0-example")
    {
        prevVersion = "2.0.1";
    }
    */

    //-------------

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Saving version file" << std::endl;

    std::string path = GetConfigFolder() + "/version";

    std::fstream file;
    file.open(path, std::fstream::out);
    file << currentVersion;
    file.close();
}