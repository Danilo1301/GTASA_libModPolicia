#include "pch.h"

#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include "Log.h"
#include "Mod.h"
#include "ModConfig.h"

#include "opcodes.h"

// ---------------------------------------

MYMODCFG(net.danilo1301.modPolicia, ModPolicia, GiroflexVSL::m_Version, Danilo1301)

// ---------------------------------------

// CLEO 2.0.1.3
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// SAUtils 1.6 (but it says 1.5.2)
#include "isautils.h"
ISAUtils* sautils = NULL;

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
    Log::file << "TestChanged - changed to " << newVal << std::endl;
}

//---------------------------------------------------------------------------------------------------


extern "C" void OnModPreLoad()
{
    ModConfig::MakePaths();

    char logPath[512];
	sprintf(logPath, "%s/modPolicia/modPolicia.log", aml->GetConfigPath());
    Log::Open(logPath);
    Log::file << "Preload()" << std::endl;
    Log::file << "AML headers: 1.0.3.1" << std::endl;

    logger->SetTag("ModPolicia");

    logger->Info("Preload");
}

extern "C" void OnModLoad()
{
    Log::file << "Load()" << std::endl;

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
    Log::file << "Loading CLEO..." << std::endl;
    cleo = (cleo_ifs_t*)GetInterface("CLEO");
    if (cleo)
    {
        Log::file << "CLEO loaded" << std::endl;
    }

    //SAUtils
    Log::file << "Loading SAUtils..." << std::endl;
    if (!(sautils = (ISAUtils*)GetInterface("SAUtils")))
    {
        Log::file << "SAUtils was not loaded" << std::endl;
    }
    else {
        Log::file << "SAUtils loaded" << std::endl;

        sautils->AddClickableItem(eTypeOfSettings::SetType_Mods, "Mod Policia - Test", 0, 0, sizeofA(optionsTest) - 1, optionsTest, TestChanged);
    }

    Log::file << "----------------------------" << std::endl;
    Log::file << "Mod: v" << Mod::m_Version << std::endl;
    Log::file << "Menu: v" << Menu::m_Version << std::endl;
    Log::file << "----------------------------" << std::endl;

    //
    
    //void* hGTASA = aml->GetLibHandle("libGTASA.so"); crashes the game
    void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);

    Log::file << "hGTASA: " << hGTASA << std::endl;

    Log::file << "Getting Syms..." << std::endl;

    SET_TO(m_vecCachedPos, aml->GetSym(hGTASA, "_ZN15CTouchInterface14m_vecCachedPosE"));
    SET_TO(pVehiclePool, aml->GetSym(hGTASA, "_ZN6CPools15ms_pVehiclePoolE"));
    SET_TO(pPedPool, aml->GetSym(hGTASA, "_ZN6CPools11ms_pPedPoolE"));

    SET_TO(ScreenGetWidth, aml->GetSym(hGTASA, "_Z17OS_ScreenGetWidthv"));
    SET_TO(ScreenGetHeight, aml->GetSym(hGTASA, "_Z18OS_ScreenGetHeightv"));
    SET_TO(GetVehicleRef, aml->GetSym(hGTASA, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(GetVehicleFromRef, aml->GetSym(hGTASA, "_ZN6CPools10GetVehicleEi"));
    SET_TO(GetPedRef, aml->GetSym(hGTASA, "_ZN6CPools9GetPedRefEP4CPed"));
    SET_TO(GetPedFromRef, aml->GetSym(hGTASA, "_ZN6CPools6GetPedEi"));
    //
    
    Log::file << "vecCachedPos: x " << m_vecCachedPos->x << ", y " << m_vecCachedPos->y << std::endl;
    Log::file << "pVehiclePool: " << pVehiclePool << std::endl;
    Log::file << "pPedPool: " << pPedPool << std::endl;
    Log::file << "ScreenGetWidth: " << ScreenGetWidth() << std::endl;
    Log::file << "ScreenGetHeight: " << ScreenGetHeight() << std::endl;

    //
    
    Log::file << "Registering opcodes..." << std::endl;

    __reg_op_func(PROCESS_MODPOLICIA_LIB, PROCESS_MODPOLICIA_LIB);
    __reg_op_func(GET_DRAW_ITEM_INFO, GET_DRAW_ITEM_INFO);
    __reg_op_func(SEND_TOUCH_STATE, SEND_TOUCH_STATE);
    __reg_op_func(ADD_LOG_MESSAGE, ADD_LOG_MESSAGE);
    
    ModConfig::ProcessVersionChanges_PreConfigLoad();
    ModConfig::Load();
    ModConfig::ProcessVersionChanges_PostConfigLoad();
    ModConfig::Save();

    Mod::Load();    
}
