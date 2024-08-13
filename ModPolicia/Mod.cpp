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
#include "Trunk.h"
#include "Stats.h"
#include "Debug.h"
#include "Load.h"

#include "systems/Skins.h"
#include "systems/Camera.h"
#include "systems/SocketServer.h"

#include "windows/WindowDocument.h"
#include "windows/WindowTest.h"
#include "windows/WindowRadio.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

extern CVector2D *m_vecCachedPos;

extern RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
extern RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);

const char* Mod::m_Version = "1.6.4";
unsigned int Mod::m_TimePassed = 0;
bool Mod::m_Enabled = false;
bool Mod::m_DevModeEnabled = false;
int Mod::m_TestWidgetId = 0;
bool Mod::m_DrawTest = true;

bool hasLoadedAnimations = false;
//bool hasLoadedModels = false;
bool hasCleoInitialized = false;

CAudioStream* test3dAudio = NULL;

bool appliedTest = false;

void Mod::Update(int dt)
{
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "Update" << std::endl;

    if(dt > 60)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "WARNING: dt was " << dt << std::endl;
        dt = 60;
    }

    m_TimePassed += dt;

    while (Draw::m_DrawItems.size() > 0)
    {
        auto dw = Draw::m_DrawItems[0];
        Draw::m_DrawItems.erase(std::find(Draw::m_DrawItems.begin(), Draw::m_DrawItems.end(), dw));
        delete dw;
    }

    //    
    
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "peds & vehicles" << std::endl;

    Peds::Update(dt);

    Vehicles::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "part1" << std::endl;
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "cleofunctions" << std::endl;

    CleoFunctions::Update(dt);

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "chase" << std::endl;

    Chase::Update(dt);

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "backup" << std::endl;

    Backup::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "part2" << std::endl;
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "pullover" << std::endl;

    Pullover::Update(dt);
   
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "scorch" << std::endl;

    Scorch::Update(dt);
   
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "callouts" << std::endl;
   
    Callouts::Update(dt);

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "ambulance" << std::endl;
   
    Ambulance::Update(dt);

    WindowDocument::Draw();
    WindowRadio::Update(dt);
    WindowRadio::Draw();

    PoliceDepartment::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "part3" << std::endl;
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "trunk" << std::endl;

    Trunk::Update(dt);

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "menu" << std::endl;

    Menu::Update(dt);
    
    Menu::Draw();

    /*
    if(m_DevModeEnabled)
        Debug::Draw();
    */

    Camera::Draw();

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "input" << std::endl;

    Input::Update(dt);

    Widgets::Update(dt);

    Mod::ProcessMenuButtons(dt);

    Stats::Update(dt);

    //

    //Draw::DrawBoxWithText(2, 1, 2, {0, 0}, {50, 50}, {255, 0, 0, 255}, {0, 0, 0, 255});

    //

    if (Menu::m_DrawCursor)
    {
        Draw::DrawGxtText(2, m_vecCachedPos->x, m_vecCachedPos->y, CVector2D(20, 300), CRGBA(255, 255, 0));
        Draw::DrawGxtText(2, (int)Menu::m_MenuOffset.x, (int)Menu::m_MenuOffset.y, CVector2D(20, 320), CRGBA(255, 255, 0));
        Draw::DrawGxtText(1, Draw::m_DrawItems.size(), 0, CVector2D(20, 340), CRGBA(255, 255, 0));
        Draw::DrawGxtText(1, dt, 0, CVector2D(20, 360), CRGBA(255, 255, 0));
    }
    
    if(m_DevModeEnabled)
    {
        auto screenSize = Input::GetGTAScreenSize();
        float x = screenSize.x/2;
        float y = 35;

        Draw::DrawGxtText(184, 0, 0, CVector2D(x, y), CRGBA(255, 255, 255));
    }

    if(CleoFunctions::PLAYER_DEFINED(0))
    {
        if(!hasCleoInitialized)
        {
            hasCleoInitialized = true;

            CleoInit();
        }

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
        auto hasRadio = false;
        auto playerActor = GetPlayerActor();

        if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor))
        {
            hasRadio = Ped::PedHasWeaponId(playerActor, 10);
        } else {
            hasRadio = CleoFunctions::GET_CURRENT_WEAPON(playerActor) == 10;
        }
        
        if(hasRadio)
        {
            if(Widgets::IsWidgetJustPressed(8)) //8 = phone
            {
                WindowRadio::ToggleRadio(true);
            }
        }
    }

    if(m_TestWidgetId > 0)
    {
        //shows widget only for testing
        Widgets::IsWidgetJustPressed(m_TestWidgetId);
    }

    if(!Menu::m_Credits->hasShownCredits)
    {
        Menu::ShowCredits(6, 5000, 80 + 50 + 10);
    }

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "Update end" << std::endl;
}

void Mod::Init()
{
    InventoryItems::Init();
    Ambulance::Init();
    PoliceDepartment::Init();

    //stats
    Stats::TimesOpenedGame += 1;
    ModConfig::SaveStats();

    SocketServer::Init();
}

CSprite2d testSprite;

void Mod::Draw()
{
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "Draw" << std::endl;

    if(!hasCleoInitialized) return;

    for(auto base : PoliceDepartment::m_Bases)
    {
        auto position = base->m_PickupPartner->position;
        auto text = "~r~[~w~BASE~r~]";

        auto playerPosition = GetPlayerPosition();

        if(DistanceBetweenPoints(playerPosition, position) < 50.0f)
        {
            menuVSL->DrawWorldText(text, position, CRGBA(255, 255, 255), eFontAlignment::ALIGN_CENTER);
        }
    }

    if(m_DrawTest)
    {
        if(!testSprite.m_pTexture)
        {
            char path[512];

            sprintf(path, "%s/assets/button_info.png", ModConfig::GetConfigFolder().c_str());
            testSprite.m_pTexture = (RwTexture*)menuVSL->LoadRwTextureFromFile(path, "test", true);
        }

        for(auto p : Peds::m_Peds)
        {
            auto ped = p.second;

            if(!CleoFunctions::ACTOR_DEFINED(ped->hPed)) continue;

            if(ped->hPed == GetPlayerActor()) continue;

            auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
            auto playerPosition = GetPedPosition(playerActor);
            auto pedPosition = GetPedPosition(ped->hPed);

            auto distance = DistanceFromPed(ped->hPed, playerPosition);
            
            auto isPedInCar = GetVehiclePedIsUsing(ped->hPed) > 0;

            if(distance < 2.0f && !isPedInCar)
            {
                std::string name = "NPC";
                CRGBA color = ped->color;

                if(Callouts::IsPedOnCriminalList(ped))
                {
                    name = "Criminal";
                    color = CRGBA(255, 0, 0);
                }
                if(PoliceDepartment::IsPedAPartner(ped) || Backup::IsPedACop(ped))
                {
                    name = "Officer";
                    color = CRGBA(0, 150, 255);
                }

                std::string text = name + " (" + std::to_string(ped->hPed) + ")";
                
                auto position = pedPosition + CVector(0, 0, 1.2f);
                
                menuVSL->DrawWorldText(text, position, color, eFontAlignment::ALIGN_CENTER);

                bool drawImage = false;
                if(drawImage)
                {
                    auto imagePos = menuVSL->ConvertWorldPositionToScreenPosition(position);
                    auto buttonSize = CVector2D(50, 50);
                    imagePos.x -= buttonSize.x/2;
                    imagePos.y -= buttonSize.y;

                    menuVSL->DrawSprite(&testSprite, imagePos, buttonSize);
                }

                bool drawHealthbar = true;
                if(drawHealthbar)
                {
                    auto barPos = menuVSL->ConvertWorldPositionToScreenPosition(position);
                    CRGBA red = CRGBA(255, 0, 0);
                    CRGBA darkRed = CRGBA(100, 0, 0);

                    auto health = CleoFunctions::ACTOR_HEALTH(ped->hPed);

                    if(health > 100) health = 100;

                    CVector2D barSize = CVector2D(120, 10);
                    CVector2D hBarSize = CVector2D(barSize.x * (health / 100.0f), barSize.y);

                    barPos.x -= barSize.x/2;
                    barPos.y += 40.0f; // TODO: change this to the font size

                    menuVSL->DrawRect(barPos, barSize, darkRed);
                    menuVSL->DrawRect(barPos, hBarSize, red);
                }

                //
            }
        }
    }

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "Draw end" << std::endl;
}

void Mod::CleoInit()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Cleo init" << std::endl;

    Debug::AddLine(1, 0);
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    //models

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading models..." << std::endl;

    AddModelsToLoad();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Showing credits" << std::endl;

    //credits

    ShowCredits();

    if(ModConfig::EnableModWhenGameStarts)
    {
        ToggleMod(true);
    }

    //

    if(m_DevModeEnabled)
    {
        ToggleMod(true);

        //WindowRadio::ToggleRadio(true);

        int modelId = 598;
        auto spawnPosition = GetPedPositionWithOffset(playerActor, CVector(0, 4.0f, 0));
        auto vehicles = Vehicles::GetAllCarsInSphere(spawnPosition, 20.0f);
        bool hasVehicle = false;
        for(auto vehicle : vehicles)
        {
            if(vehicle->modelId == modelId)
            {
                hasVehicle = true;
                break;
            }
        }
        if(!hasVehicle)
        {
            auto carHandle = CleoFunctions::CREATE_CAR_AT(modelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);
            CleoFunctions::SET_CAR_DOOR_STATUS(carHandle, 1);
            //CleoFunctions::ENABLE_CAR_SIREN(carHandle, true); //annoying
        }

        CleoFunctions::CHANGE_PLAYER_MODEL_TO(0, 280);

        ModConfig::CreateTestOptionsInRadioMenu = true;
    }

    if(ModConfig::StartGameWithRadio)
    {
        if(!Ped::PedHasWeaponId(playerActor, 10))
        {
            CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 10, 1);
        }
    }

    //


    //

    /*
    //test audio
    std::string audiosPath = ModConfig::GetConfigFolder() + "/audios/";
    auto audioStream = SoundSystem::LoadStream(audiosPath + "/ht.wav", false);
    Log::Level(LOG_LEVEL::LOG_BOTH) << "audioStream: " << audioStream << std::endl;
    audioStream->Play();
    */
}

void Mod::AddModelsToLoad()
{
    Load::AddModelToLoad(596); //copcarla
    Load::AddModelToLoad(280); //ls

    Load::AddModelToLoad(597); //copcarsf
    Load::AddModelToLoad(281); //sf

    Load::AddModelToLoad(598); //copcarvg
    Load::AddModelToLoad(282); //lv

    Load::AddModelToLoad(599); //ranger
    Load::AddModelToLoad(283); //csher

    Load::AddModelToLoad(523); //copbike
    Load::AddModelToLoad(284); //lapdm1

    Load::AddModelToLoad(490); //fbiranch
    Load::AddModelToLoad(286); //fbi

    Load::AddModelToLoad(497); //police maverick

    Load::AddModelToLoad(525); //towtruck
    Load::AddModelToLoad(50); //wmymech

    Load::AddModelToLoad(416); //ambulance
    Load::AddModelToLoad(274); //laemt1

    Load::AddModelToLoad(442); //romero
    Load::AddModelToLoad(70); //laemt1

    Load::AddModelToLoad(528); //fbitruck
    Load::AddModelToLoad(288); //dsher

    Load::AddModelToLoad(601); //swat
    Load::AddModelToLoad(285); //swatvan

    //skins
    for(auto skin : Skins::m_Skins)
    {
        Load::AddModelToLoad(skin.modelId);
    }

    for(auto skin : PoliceDepartment::m_PartnerSkins)
    {
        Load::AddModelToLoad(skin.pedModelId);
    }

    //stolen vehicles
    for(auto id : Callouts::m_StolenVehicleIds)
    {
        Load::AddModelToLoad(id);
    }
    for(auto id : Callouts::m_StolenTruckIds)
    {
        Load::AddModelToLoad(id);
    }

    //weapons
    Load::AddModelToLoad(335); //knife
    Load::AddModelToLoad(367); //camera

    for(auto weapon : PoliceDepartment::m_Weapons)
    {
        Load::AddModelToLoad(weapon.weaponModelId);
    }

    //objects
    Load::AddModelToLoad(1459); //barrier
    Load::AddModelToLoad(2899); //spikes
    Load::AddModelToLoad(2328); //small box

    //pickups
    //https://www.open.mp/docs/scripting/resources/pickupids
    //Load::AddModelToLoad(1210); //Briefcase
    //Load::AddModelToLoad(1239); //Information
    //Load::AddModelToLoad(1242); //Body armour
    //Load::AddModelToLoad(1314); //Two-player (crashes for Leonardo Alves)

    for(auto base : PoliceDepartment::m_Bases)
    {
        Load::AddModelToLoad(base->m_PickupDuty->pickupModelId);
        Load::AddModelToLoad(base->m_PickupEquipment->pickupModelId);
        Load::AddModelToLoad(base->m_PickupPartner->pickupModelId);
    }
    
    Load::LoadAll([]() {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: All models loaded!" << std::endl;
    });
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

/*
if(CleoFunctions::IS_CHAR_IN_ANY_CAR(GetPlayerActor()) && !appliedTest)
{
    appliedTest = true;

    auto carHandle = CleoFunctions::ACTOR_USED_CAR(GetPlayerActor());

    auto vehicle = Vehicles::GetVehicleByHandle(carHandle);
    auto pVehicle = vehicle->pVehicle;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "placement: " << CVectorToString(pVehicle->GetPlacement()->pos) << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "matrix: " << CVectorToString(pVehicle->GetMatrix()->at) << std::endl;


    Log::Level(LOG_LEVEL::LOG_BOTH) << "pos: " << CVectorToString(pVehicle->m_placement.pos) << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "at: " << CVectorToString(pVehicle->m_matrix->at) << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "m_pRwClump: " << pVehicle->m_pRwClump << std::endl;

    auto clump = pVehicle->m_pRwClump;

    RpClumpForAllAtomics(clump, [](RpAtomic* atomic, void* data) {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "atomic: " << atomic << std::endl;

        if (atomic->geometry)
        {
            auto geometry = atomic->geometry;

            
            RpGeometryForAllMaterials(geometry, [](RpMaterial* material, void* data) {
                
                Log::Level(LOG_LEVEL::LOG_BOTH) << "material: " << material << std::endl;

                material->color = { 255, 0, 0, 255 };

                return material;
            }, 0);
            
        }

        return atomic;
    }, (void*)((uint32_t)(0)));
}
*/