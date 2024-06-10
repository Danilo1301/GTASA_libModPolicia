#include "pch.h"

#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include "Log.h"
#include "Mod.h"
#include "ModConfig.h"
#include "SoundSystem.h"

#include "opcodes.h"

// ---------------------------------------

MYMODCFG(net.danilo1301.modPolicia, ModPolicia, 1.2.0, Danilo1301)

// ---------------------------------------

// CLEO 2.0.1.5
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// SAUtils 1.6 (but it says 1.5.2)
#include "isautils.h"
ISAUtils* sautils = NULL;

// BASS
IBASS* BASS = NULL;

// ---------------------------------------

CVector2D *m_vecCachedPos;
uintptr_t* pVehiclePool = 0;
uintptr_t* pPedPool = 0;

int (*ScreenGetWidth)();
int (*ScreenGetHeight)();
int (*GetVehicleRef)(int);
void* (*GetVehicleFromRef)(int);
int (*GetPedRef)(int);
void* (*GetPedFromRef)(int);

CCamera* camera;
bool* userPaused;
bool* codePaused;

// ---------------------------------------

DECL_HOOK(void*, UpdateGameLogic, uintptr_t a1)
{
    if (BASS) {
        SoundSystem::Update();
    }

    return UpdateGameLogic(a1);
}

// ---------------------------------------

std::string CheckModVersion(std::vector<std::string> GUIDs, std::vector<std::string> versions)
{
    for (int ii = GUIDs.size() - 1; ii >= 0; ii--)
    {
        auto GUID = GUIDs[ii];

        for (int i = versions.size() - 1; i >= 0; i--)
        {
            std::string version = versions[i];

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Checking " << GUID << " " << version << "..." << std::endl;

            if (aml->HasModOfVersion(GUID.c_str(), version.c_str()))
                return version;
        }
    }
    return "";
}

// ---------------------------------------

ConfigEntry* cfgMenuOffsetX = NULL;

//---------------------------------------------------------------------------------------------------
//test

const char* optionsTest[] = {
    "test1",
    "test2"
};
void TestChanged(int oldVal, int newVal, void* data)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "TestChanged - changed to " << newVal << std::endl;
}

//---------------------------------------------------------------------------------------------------

extern "C" void OnModPreLoad()
{
    ModConfig::MakePaths();

    char logPath[512];
	sprintf(logPath, "%s/modPolicia/", aml->GetConfigPath());
    Log::Open(logPath);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Preload()" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "AML headers: 1.0.3.1" << std::endl;

    logger->SetTag("ModPolicia");

    logger->Info("Preload");
}

extern "C" void OnModLoad()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load()" << std::endl;

    cfgMenuOffsetX = cfg->Bind("menu_offset_x", -195, "General");
    //cfgTimeBetweenPatterns = cfg->Bind("time_between_patterns", Patterns::m_TimeBetweenPatterns, "General");
    //cfgCoronaFpsFix = cfg->Bind("corona_fps_fix", Mod::CoronaFixFPS, "General");

    Menu::m_MenuOffset.x = (float)cfgMenuOffsetX->GetInt();
    //Patterns::m_TimeBetweenPatterns = cfgTimeBetweenPatterns->GetInt();
    //Mod::CoronaFixFPS = (eCoronaFixFPS)cfgCoronaFpsFix->GetInt();

    cfg->Bind("Author", "", "About")->SetString("Danilo1301"); cfg->ClearLast();
    cfg->Bind("Discord", "", "About")->SetString("https://discord.gg/mkCDRf4zJA"); cfg->ClearLast();
    cfg->Bind("GitHub", "", "About")->SetString("https://github.com/Danilo1301/GTASA_libModPolicia"); cfg->ClearLast();
    cfg->Save();

    //CLEO
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading CLEO..." << std::endl;
    cleo = (cleo_ifs_t*)GetInterface("CLEO");
    if (cleo)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "CLEO loaded" << std::endl;
    }

    //SAUtils
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading SAUtils..." << std::endl;
    if (!(sautils = (ISAUtils*)GetInterface("SAUtils")))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "SAUtils was not loaded" << std::endl;
    }
    else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "SAUtils loaded" << std::endl;

        sautils->AddClickableItem(eTypeOfSettings::SetType_Mods, "Mod Policia - Test", 0, 0, sizeofA(optionsTest) - 1, optionsTest, TestChanged);
    }

    //BASS
    //https://github.com/AndroidModLoader/GTASA_CLEO_AudioStreams
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading BASS..." << std::endl;
    if (!(BASS = (IBASS*)GetInterface("BASS")))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "BASS was not loaded" << std::endl;
    }
    else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "BASS loaded: " << BASS << std::endl;

        SoundSystem::Init();

        /*
        std::string audiosPath = ModConfig::GetConfigFolder() + "/audios/";

        Log::Level(LOG_LEVEL::LOG_BOTH) << "Playing test audio" << std::endl;

        SoundSystem::PlayStreamFromAudiosFolder("ht.wav", false);
        SoundSystem::PlayStreamFromAudiosFolder("static.wav", false);
        */

        /*
        auto audioStream = SoundSystem::LoadStream(audiosPath + "/ht.wav", false);
        audioStream->Loop(true);
        audioStream->Play();
        */
    }

    std::string cleoVersion = CheckModVersion(
        { "net.rusjj.cleolib", "net.rusjj.cleomod" },
        { "2.0.1", "2.0.1.1", "2.0.1.2", "2.0.1.3", "2.0.1.4", "2.0.1.5" }
    );

    std::string sautilsVersion = CheckModVersion(
        { "net.rusjj.gtasa.utils" },
        { "1.1", "1.2", "1.2.1", "1.3.0", "1.3.1", "1.4", "1.4.1", "1.5.1", "1.6"}
    );
    
    std::string amlVersion = CheckModVersion(
        { "net.rusjj.aml" },
        { "1.0.0.0", "1.0.0.1", "1.0.0.2", "1.0.0.3", "1.0.0.4", "1.0.0.5", "1.0.0.6", "1.0.1", "1.0.2", "1.0.2.1", "1.0.2.2", "1.0.3", "1.0.3.1", "1.1", "1.2", "1.2.1", "1.2.2"}
    );

    Log::Level(LOG_LEVEL::LOG_BOTH) << "----------------------------" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod Policia: v" << Mod::m_Version << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Menu: v" << Menu::m_Version << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "CLEO version: " << cleoVersion << " (recommended 2.0.1.3)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "SAUtils version: " << sautilsVersion << " (recommended 1.3.1)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "AML version: " << amlVersion << " (recommended 1.2.2)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "----------------------------" << std::endl;

    //
    
    //void* hGTASA = aml->GetLibHandle("libGTASA.so"); crashes the game
    void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t gameAddr = (uintptr_t)(cleo->GetMainLibraryLoadAddress());

    Log::Level(LOG_LEVEL::LOG_BOTH) << "hGTASA: " << hGTASA << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Getting Syms..." << std::endl;

    SET_TO(m_vecCachedPos, aml->GetSym(hGTASA, "_ZN15CTouchInterface14m_vecCachedPosE"));
    SET_TO(pVehiclePool, aml->GetSym(hGTASA, "_ZN6CPools15ms_pVehiclePoolE"));
    SET_TO(pPedPool, aml->GetSym(hGTASA, "_ZN6CPools11ms_pPedPoolE"));

    SET_TO(ScreenGetWidth, aml->GetSym(hGTASA, "_Z17OS_ScreenGetWidthv"));
    SET_TO(ScreenGetHeight, aml->GetSym(hGTASA, "_Z18OS_ScreenGetHeightv"));
    SET_TO(GetVehicleRef, aml->GetSym(hGTASA, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(GetVehicleFromRef, aml->GetSym(hGTASA, "_ZN6CPools10GetVehicleEi"));
    SET_TO(GetPedRef, aml->GetSym(hGTASA, "_ZN6CPools9GetPedRefEP4CPed"));
    SET_TO(GetPedFromRef, aml->GetSym(hGTASA, "_ZN6CPools6GetPedEi"));

    SET_TO(camera, cleo->GetMainLibrarySymbol("TheCamera"));
    SET_TO(userPaused, cleo->GetMainLibrarySymbol("_ZN6CTimer11m_UserPauseE"));
    SET_TO(codePaused, cleo->GetMainLibrarySymbol("_ZN6CTimer11m_CodePauseE"));
    
    HOOKPLT(UpdateGameLogic, gameAddr + 0x66FE58);

    //
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "vecCachedPos: x " << m_vecCachedPos->x << ", y " << m_vecCachedPos->y << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "pVehiclePool: " << pVehiclePool << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "pPedPool: " << pPedPool << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ScreenGetWidth: " << ScreenGetWidth() << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ScreenGetHeight: " << ScreenGetHeight() << std::endl;

    //
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Registering opcodes..." << std::endl;

    __reg_op_func(PROCESS_MODPOLICIA_LIB, PROCESS_MODPOLICIA_LIB);
    __reg_op_func(GET_DRAW_ITEM_INFO, GET_DRAW_ITEM_INFO);
    __reg_op_func(SEND_TOUCH_STATE, SEND_TOUCH_STATE);
    __reg_op_func(ADD_LOG_MESSAGE, ADD_LOG_MESSAGE);
    
    ModConfig::DefineVersions();
    ModConfig::ProcessVersionChanges_PreConfigLoad();
    ModConfig::Load();
    ModConfig::ProcessVersionChanges_PostConfigLoad();
    ModConfig::Save();

    Mod::Init();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Mod loaded" << std::endl;
}
