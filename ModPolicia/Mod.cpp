#include "Mod.h"

#include "menu/Draw.h"
#include "menu/Menu.h"
#include "Input.h"
#include "Widgets.h"
#include "Log.h"
#include "Peds.h"
#include "Pullover.h"
#include "CleoFunctions.h"
#include "InventoryItem.h"
#include "Chase.h"
#include "ModConfig.h"
#include "Callouts.h"
#include "Vehicles.h"
#include "Scorch.h"
#include "Backup.h"
#include "Ambulance.h"
#include "PoliceDepartment.h"
#include "SoundSystem.h"

#include "windows/WindowDocument.h"
#include "windows/WindowTest.h"
#include "windows/WindowRadio.h"

extern CVector2D *m_vecCachedPos;

const char* Mod::m_Version = "1.3.0";
unsigned int Mod::m_TimePassed = 0;
bool Mod::m_Enabled = false;

bool hasLoadedAnimations = false;
//bool hasLoadedModels = false;
bool hasCleoInitialized = false;

CAudioStream* test3dAudio = NULL;

std::vector<int> modelsToLoad;
//std::vector<int> loadedModels;

void Mod::Update(int dt)
{
    //Log::file << "Mod::Update" << std::endl;

    m_TimePassed += dt;

    while (Draw::m_DrawItems.size() > 0)
    {
        auto dw = Draw::m_DrawItems[0];
        Draw::m_DrawItems.erase(std::find(Draw::m_DrawItems.begin(), Draw::m_DrawItems.end(), dw));
        delete dw;
    }

    //    

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "peds ------------------" << std::endl;

    Peds::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "vehicles" << std::endl;

    Vehicles::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "chase" << std::endl;

    Chase::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "backup" << std::endl;

    Backup::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "pullover" << std::endl;

    Pullover::Update(dt);
   
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "scorch" << std::endl;

    Scorch::Update(dt);
   
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "callouts" << std::endl;
   
    Callouts::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "ambulance" << std::endl;
   
    Ambulance::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "cleofuncitions" << std::endl;

    CleoFunctions::Update(dt);

    WindowDocument::Draw();

    PoliceDepartment::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "menu" << std::endl;

    Menu::Update(dt);
    
    Menu::Draw();

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "input" << std::endl;

    Input::Update(dt);

    Widgets::Update(dt);

    Mod::ProcessMenuButtons(dt);

    //

    //Draw::DrawBoxWithText(2, 1, 2, {0, 0}, {50, 50}, {255, 0, 0, 255}, {0, 0, 0, 255});

    //

    if (Menu::m_DrawCursor)
    {
        Draw::DrawText(2, m_vecCachedPos->x, m_vecCachedPos->y, CVector2D(20, 300), CRGBA(255, 255, 0));
        Draw::DrawText(2, (int)Menu::m_MenuOffset.x, (int)Menu::m_MenuOffset.y, CVector2D(20, 320), CRGBA(255, 255, 0));
        Draw::DrawText(1, Draw::m_DrawItems.size(), 0, CVector2D(20, 340), CRGBA(255, 255, 0));
        Draw::DrawText(1, dt, 0, CVector2D(20, 360), CRGBA(255, 255, 0));
    }

    if(CleoFunctions::PLAYER_DEFINED(0))
    {
        if(!hasCleoInitialized)
        {
            hasCleoInitialized = true;

            CleoInit();
        }

        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        /*
        if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor) && test3dAudio == NULL)
        {
            auto carHandle = CleoFunctions::ACTOR_USED_CAR(playerActor);

            auto car = Vehicles::TryCreateVehicle(carHandle);

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Attach test audio" << std::endl;

            std::string audiosPath = ModConfig::GetConfigFolder() + "/audios/";
            test3dAudio = SoundSystem::LoadStream(audiosPath + "/siren3-mono.wav", true);
            Log::Level(LOG_LEVEL::LOG_BOTH) << "test3dAudio: " << test3dAudio << std::endl;
            test3dAudio->Loop(true);
            test3dAudio->Link(car->pVehicle);
            test3dAudio->Play();
        }
        */

        if(!hasLoadedAnimations)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Checking for animations..." << std::endl;

            if(
                CleoFunctions::HAS_ANIMATION_LOADED("POLICE") &&
                CleoFunctions::HAS_ANIMATION_LOADED("GANGS") &&
                CleoFunctions::HAS_ANIMATION_LOADED("MEDIC")
            )
            {
                hasLoadedAnimations = true;

                Log::Level(LOG_LEVEL::LOG_BOTH) << "Animations are already loaded!" << std::endl;
            } else {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading animations..." << std::endl;

                CleoFunctions::LOAD_ANIMATION("GANGS");
                CleoFunctions::LOAD_ANIMATION("POLICE");
                CleoFunctions::LOAD_ANIMATION("MEDIC");
            }
        }
    }

    if(m_Enabled)
    {
        if(Widgets::IsWidgetJustPressed(8)) //8 = phone
        {
            WindowRadio::Create();
        }
    }

    /*
    if(Widgets::IsWidgetJustPressed(38)) //POWER
    {
        Log::file << "IS_WIDGET_PRESSED 38 is true!" << std::endl;

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0.0f, y = 0.0f, z = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &x, &y, &z);

        int intX = (int)x;
        int intY = (int)y;
        CleoFunctions::SHOW_TEXT_2NUMBERS_STYLED("MPFX2", intX, intY, 3000, 5);

        int randomChar = CleoFunctions::GET_RANDOM_CHAR_IN_SPHERE(x, y, z, 2.0f, true, true, true);
        
        Log::file << "randomChar: " << randomChar << std::endl;

        CleoFunctions::SHOW_TEXT_2NUMBERS_STYLED("MPFX1", randomChar, 0, 3000, 5);

        if(randomChar > 0)
        {
            Log::file << "ADD_BLIP_FOR_CHAR: " << randomChar << std::endl;
            CleoFunctions::ADD_BLIP_FOR_CHAR(randomChar);
        }
    }
    */

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "end ---------" << std::endl;
}

void Mod::Init()
{
    InventoryItems::Init();
    Ambulance::Init();
    PoliceDepartment::Init();
}

void Mod::CleoInit()
{
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    //models

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading models..." << std::endl;

    RequestModelsToLoad();

    //credits

    ShowCredits();

    //

    /*
    //test audio
    std::string audiosPath = ModConfig::GetConfigFolder() + "/audios/";
    auto audioStream = SoundSystem::LoadStream(audiosPath + "/ht.wav", false);
    Log::Level(LOG_LEVEL::LOG_BOTH) << "audioStream: " << audioStream << std::endl;
    audioStream->Play();
    */
}

void Mod::RequestModelsToLoad()
{
    AddModelToLoad(596); //copcarla
    AddModelToLoad(280); //ls

    AddModelToLoad(597); //copcarsf
    AddModelToLoad(281); //sf

    AddModelToLoad(598); //copcarvg
    AddModelToLoad(282); //lv

    AddModelToLoad(599); //ranger
    AddModelToLoad(283); //csher

    AddModelToLoad(523); //copbike
    AddModelToLoad(284); //lapdm1

    AddModelToLoad(490); //fbiranch
    AddModelToLoad(286); //fbi

    AddModelToLoad(497); //police maverick

    AddModelToLoad(525); //towtruck
    AddModelToLoad(50); //wmymech

    AddModelToLoad(416); //ambulance
    AddModelToLoad(274); //laemt1

    AddModelToLoad(442); //romero
    AddModelToLoad(70); //laemt1

    AddModelToLoad(528); //fbitruck
    AddModelToLoad(288); //dsher

    AddModelToLoad(601); //swat
    AddModelToLoad(285); //swatvan

    //skins
    for(auto skin : Callouts::m_Skins)
    {
        AddModelToLoad(skin.modelId);
    }

    for(auto skin : PoliceDepartment::m_PartnerSkins)
    {
        AddModelToLoad(skin.pedModelId);
    }

    //stolen vehicles
    for(auto id : Callouts::m_StolenVehicleIds)
    {
        AddModelToLoad(id);
    }
    for(auto id : Callouts::m_StolenTruckIds)
    {
        AddModelToLoad(id);
    }

    //weapons
    AddModelToLoad(335); //knife
    AddModelToLoad(367); //camera

    for(auto weapon : PoliceDepartment::m_Weapons)
    {
        AddModelToLoad(weapon.weaponModelId);
    }

    //objects
    AddModelToLoad(1459); //barrier
    AddModelToLoad(2899); //spikes

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: loading models..." << std::endl;

    LoadRequestedModels([] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: Models loaded!" << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: Loading pickups..." << std::endl;

        //pickups
        //https://www.open.mp/docs/scripting/resources/pickupids
        AddModelToLoad(1210); //Briefcase
        AddModelToLoad(1239); //Information
        AddModelToLoad(1242); //Body armour
        AddModelToLoad(1314); //Two-player
        
        LoadRequestedModels([] () {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: Pickups loaded!" << std::endl;
        });
    });

    
}

void Mod::LoadRequestedModels(std::function<void()> callback)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: load requested models" << std::endl;

    CleoFunctions::LOAD_REQUESTED_MODELS();

    CleoFunctions::AddWaitForFunction([] () {
        return CheckModelsLoaded();
    }, [callback] () {
        callback();
    });
}

void Mod::AddModelToLoad(int modelId)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: loading model " << modelId << std::endl;

    modelsToLoad.push_back(modelId);

    CleoFunctions::LOAD_MODEL(modelId);

    /*
    CleoFunctions::AddWaitForFunction([modelId] () {
        return CleoFunctions::MODEL_AVAILABLE(modelId);
    }, [modelId] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: Model " << modelId << " loaded!" << std::endl;
    });
    */
}

bool Mod::CheckModelsLoaded()
{
    if(modelsToLoad.size() == 0) return true;

    std::vector<int> newLoadedModels;
    for(auto modelId : modelsToLoad)
    {
        if(CleoFunctions::MODEL_AVAILABLE(modelId))
        {
            newLoadedModels.push_back(modelId);
            
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Model " << modelId << " has been loaded" << std::endl;
        }
    }

    for(auto modelId : newLoadedModels)
    {
        auto it = std::find(modelsToLoad.begin(), modelsToLoad.end(), modelId);
        modelsToLoad.erase(it);

        //loadedModels.push_back(modelId);
    }

    return false;
}

int Mod::GetRandomNumber(int min, int max)
{
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do{
        x = rand();
    }while (x >= RAND_MAX - remainder);
    return min + x % n;
}

bool Mod::CalculateProbability(float chance)
{
    int i = GetRandomNumber(0, 100);
    return i <= (int)(chance * 100.0f);
}

void Mod::ProcessMenuButtons(int dt)
{
    if(ModConfig::EnableTestMenu)
    {
        //test menu, 2 and 8 (LEFT, --, RIGHT)
        if (Input::GetTouchIdState(2) && Input::GetTouchIdState(8))
        {
            if (Input::GetTouchIdPressTime(8) > 500)
            {
                WindowTest::Create();
            }
        }
    }

    //test menu, 5 and 6
    if (Input::GetTouchIdState(6) && Input::GetTouchIdState(5))
    {
        if (Input::GetTouchIdPressTime(6) > 500)
        {
            
        }
    }
}

CVector Mod::GetCarPositionWithOffset(int hVehicle, CVector offset)
{
    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(hVehicle, offset.x, offset.y, offset.z, &x, &y, &z);

    return CVector(x, y, z);
}

CVector Mod::GetCarPosition(int hVehicle)
{
    return GetCarPositionWithOffset(hVehicle, CVector(0, 0, 0));
}

CVector Mod::GetPedPositionWithOffset(int hPed, CVector offset)
{
    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(hPed, offset.x, offset.y, offset.z, &x, &y, &z);

    return CVector(x, y, z);
}

CVector Mod::GetPedPosition(int hPed)
{
    return GetPedPositionWithOffset(hPed, CVector(0, 0, 0));
}

double Mod::DistanceFromPed(int hPed, CVector position)
{
    auto pedPosition = GetPedPosition(hPed);
    auto distance = DistanceBetweenPoints(pedPosition, position);
    return distance;
}

bool Mod::IsActorAliveAndDefined(int hPed)
{
    return CleoFunctions::ACTOR_DEFINED(hPed) && !CleoFunctions::ACTOR_DEAD(hPed);
}

void Mod::ToggleMod(bool enabled)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Toggle mod: " << (enabled ? "Enabled" : "Disabled") << std::endl;

    m_Enabled = enabled;

    //auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    if(enabled)
    {
        CleoFunctions::SET_PLAYER_IGNORED_BY_COPS(0, true);
        CleoFunctions::SET_MAX_WANTED_LEVEL_TO(0);

        ShowCredits();
    } else {
        CleoFunctions::SET_PLAYER_IGNORED_BY_COPS(0, false);
        CleoFunctions::SET_MAX_WANTED_LEVEL_TO(6);
    }
    CleoFunctions::SET_PLAYER_WANTED_LEVEL(0, 0);
}

void Mod::ShowCredits()
{
    CleoFunctions::SHOW_TEXT_BOX("MPFX140"); //mod policia credits
}