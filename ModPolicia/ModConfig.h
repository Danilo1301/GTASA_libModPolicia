#pragma once

#include "pch.h"

#include "iniconfig/INISection.h"

struct VersionInfo {
	std::string version = "";
	std::vector<std::function<void()>> patches;
};

class VersionControl {
public:
	static std::vector<VersionInfo*> m_Versions;
	static std::string m_PrevVersion;
	static std::string m_CurrentVersion;

	static void SetVersion(std::string prevVersion, std::string currentVersion);
	static void AddVersion(std::string version);
	static VersionInfo* GetVersionInfo(std::string version);

	static void AddPatch(std::string version, std::function<void()> patch);
	static void AddPatch(std::function<void()> patch);
	static void ApplyPatches();
};

class ModConfig {
public:
	static std::string m_ConfigMainFolderName;
    static bool EnableTestMenu;
    static bool CreateTestOptionsInRadioMenu;
    static bool EnableModWhenGameStarts;
    static bool StartGameWithRadio;

	static void MakePaths();
	static bool DirExists(std::string path);
	static bool FileExists(std::string path);
	static std::vector<std::string> GetDirectoriesName(std::string path);
	static std::vector<std::string> GetFilesName(std::string path);
	static void ConfigDelete(std::string path);

	static std::string GetConfigFolder();
	static void CreateFolder(std::string path);

	static void WriteToFile(std::string path, Json::Value value);
	static Json::Value ReadFile(std::string path);
	
	static void Save();
	static void SaveSettings();
	static void SaveDataSettings();
	static void SaveStats();

	static void Load();
	static void LoadSettings();
	static void LoadDataSettings();
	static void LoadStats();

    static std::string ReadVersionFile();
	static void DefineVersions();
	static void ProcessVersionChanges_PreConfigLoad();
	static void ProcessVersionChanges_PostConfigLoad();
};