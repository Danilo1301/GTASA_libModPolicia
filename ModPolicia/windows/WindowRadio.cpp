#include "WindowRadio.h"

#include "CleoFunctions.h"
#include "Mod.h"
#include "Ambulance.h"
#include "Callouts.h"
#include "SoundSystem.h"
#include "Chase.h"
#include "Pullover.h"
#include "ModConfig.h"
#include "Peds.h"
#include "Log.h"
#include "Vehicles.h"
#include "Scorch.h"
#include "Widgets.h"
#include "Backup.h"
#include "WindowBackup.h"
#include "Debug.h"

#include "systems/Camera.h"

Window* WindowRadio::m_WindowMenu = NULL;
bool WindowRadio::m_Enabled = false;
CVector2D WindowRadio::m_Position = CVector2D(280, 210);
bool WindowRadio::m_TransparentButtons = false;
unsigned char WindowRadio::m_TransparentButtonAlpha = 20;

Window* WindowRadio::m_WindowRadio = NULL;
Item* WindowRadio::m_LeftButton = NULL;
Item* WindowRadio::m_RightButton = NULL;
Item* WindowRadio::m_OkButton = NULL;
Item* WindowRadio::m_BackButton = NULL;

std::vector<RadioChannel> WindowRadio::m_Channels = {
    {103, { {104}, {105}, {106} }},
    {107, { {108}, {109} }},
    {110, { {111}, {112}, {113} }},
    {114, { {115}, {116}, {117} }},
    {118, { {119}, {120}, {121} }},
    {122, {}},
    {123, {}}
};
int WindowRadio::m_CurrentChannel = 0;
int WindowRadio::m_CurrentFrequency = 0;
bool WindowRadio::m_ChangingChannels = true;

int radioObject = 0;
int imlCar = 0;
int imlPed = 0;

#include "isautils.h"
extern ISAUtils* sautils;
int scmHandle1, scmHandle2;
float scmHandleFloat1, scmHandleFloat2, scmHandleFloat3;
static DEFOPCODE(01F5, GET_PLAYER_ACTOR, iv); //01F5: $PLAYER_ACTOR = get_player_actor $PLAYER_CHAR 
static DEFOPCODE(00A0, GET_CHAR_COORDINATES, ivvv);
static DEFOPCODE(036D, SHOW_TEXT_2NUMBERS_STYLED, siiii);
static DEFOPCODE(0256, IS_PLAYER_PLAYING, i); //0256:  player $PLAYER_CHAR defined 

int testStep = 0;

void WindowRadio::Create()
{
	if (m_WindowMenu) return;

    auto window = m_WindowMenu = Menu::AddWindow(6);
    window->showPageControls = true;

    auto button_abortCallout = window->AddButton(119, 0, 0);
    button_abortCallout->onClick = []()
    {
        Remove();
        Callouts::AbortCallout();
    };

    auto button_config = window->AddButton(107);
    button_config->onClick = [window]()
    {
        WindowBackup::CreateBackupConfig(window);
    };

    if(ModConfig::CreateTestOptionsInRadioMenu)
    {
        CreateTestOptions();
    }

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowRadio::Remove()
{
    m_WindowMenu->RemoveThisWindow();
    m_WindowMenu = NULL;
}

void WindowRadio::CreateTestOptions()
{
    auto window = m_WindowMenu;
    
    auto button_pedPullover = window->AddButton(127, 0, 0);
    button_pedPullover->onClick = []()
    {
        Remove();
        Pullover::CreateTestPullOverPed();
    };

    auto button_carPullover = window->AddButton(128, 0, 0);
    button_carPullover->onClick = []()
    {
        Remove();
        Pullover::CreateTestPullOverVehicle();
    };

    auto button_chaseEnd = window->AddButton(129, 0, 0);
    button_chaseEnd->onClick = []()
    {
        Remove();
        if(Chase::m_ChasingPed)
        {
            auto vehicle = Vehicles::GetVehicleByHandle(Chase::m_ChasingPed->hVehicleOwned);
            //Chase::EndChase();
            CleoFunctions::CLEAR_ACTOR_TASK(Chase::m_ChasingPed->hPed);
            CleoFunctions::EXIT_CAR_AS_ACTOR(Chase::m_ChasingPed->hPed);
            //vehicle->MakePedsExitCar();
            //Pullover::m_PullingVehicle = vehicle;
            //Pullover::m_PullingPed = Peds::GetPedByHandle(vehicle->GetDriver());
            //Pullover::AskPedsToLeaveCar(vehicle);
        }
    };

    auto button_freezeCarChase = window->AddButton(214, 9, 0);
    button_freezeCarChase->onClick = []()
    {
        Remove();

        auto chasingPed = Chase::m_ChasingPed;

        if(chasingPed) {
            auto vehicle = Vehicles::GetVehicleByHandle(chasingPed->hVehicleOwned);
            vehicle->freezeCarPosition = !vehicle->freezeCarPosition;
        }
    };

    auto button_test10 = window->AddButton(23, 10, 0);
    button_test10->onClick = []()
    {
        Remove();

        Debug::AddLine(1, testStep);

        if(testStep == 0)
        {
            auto playerActor = Mod::GetPlayerActor();
            auto spawnPosition = Mod::GetPedPositionWithOffset(playerActor, CVector(0, 5, 0));

            imlCar = CleoFunctions::CREATE_CAR_AT(442, spawnPosition.x, spawnPosition.y, spawnPosition.z);
        }

        if(testStep == 1)
        {
            auto playerActor = Mod::GetPlayerActor();
            auto spawnPosition = Mod::GetPedPositionWithOffset(playerActor, CVector(0, 5, 0));
            
            imlPed = CleoFunctions::CREATE_ACTOR_PEDTYPE(23, 70, spawnPosition.x, spawnPosition.y, spawnPosition.z);
        }

        if(testStep == 2)
        {
            //CleoFunctions::ENTER_CAR_AS_DRIVER_AS_ACTOR(imlPed, imlCar, 10000);

            auto ped = Peds::TryCreatePed(imlPed);
            //ped->SetGoToVehicle(imlCar, 0, true);

            //auto carPosition = Mod::GetCarPosition(imlCar);

            //CleoFunctions::ACTOR_GOTO_POINT(imlPed, carPosition.x, carPosition.y, carPosition.z, 6, -1);
        }

        if(testStep == 3)
        {
            testStep = 0;
            return;
        }

        testStep++;
    };

    auto button_test9 = window->AddButton(23, 9, 0);
    button_test9->onClick = []()
    {
        Remove();

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0, y = 0, z = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 10, 0, &x, &y, &z);

        auto car = CleoFunctions::CREATE_CAR_AT(596, x, y, z);

        CleoFunctions::ENABLE_CAR_SIREN(car, true);

        int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 4, 284);
    };

    auto button_test7 = window->AddButton(23, 7, 0);
    button_test7->onClick = []()
    {
        Remove();

        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        auto position = Mod::GetPedPositionWithOffset(playerActor, CVector(0, 2, 0));

        auto pedHandle = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, 19, position.x, position.y, position.z);
        auto ped = Peds::TryCreatePed(pedHandle);
        ped->AddBlip();

        Callouts::AddPedToCriminalList(ped);
    };

    auto button_test8 = window->AddButton(23, 8, 0);
    button_test8->onClick = []()
    {
        Remove();

        auto chasingPed = Chase::m_ChasingPed;

        if(chasingPed) Chase::DeflateCarTires(chasingPed->hVehicleOwned);
    };
    
    auto button_test6 = window->AddButton(23, 6, 0);
    button_test6->onClick = []()
    {
        Remove();

        auto peds = Peds::GetDeadPeds();

        for(auto ped : peds)
        {
            auto pedPosition = Mod::GetPedPosition(ped->hPed);

            auto sphere = CleoFunctions::CREATE_SPHERE(pedPosition.x, pedPosition.y, pedPosition.z, 3.0);
        }

        Log::Level(LOG_LEVEL::LOG_BOTH) << peds.size() << " dead peds found" << std::endl;
    };

    auto button_test1 = window->AddButton(23, 1, 0);
    button_test1->onClick = []()
    {
        Remove();

        /*
        Log::file << "scm_GET_PLAYER_ACTOR" << std::endl;
        scmHandle1 = 0;
        sautils->ScriptCommand(&scm_GET_PLAYER_ACTOR, 0, &scmHandle1);
        Log::file << "scmHandle1 = " << scmHandle1 << std::endl;
        int playerActor = scmHandle1;

        Log::file << "scm_GET_CHAR_COORDINATES" << std::endl;
        scmHandleFloat1 = 0.0;
        scmHandleFloat2 = 0.0;
        scmHandleFloat3 = 0.0;
        sautils->ScriptCommand(&scm_GET_CHAR_COORDINATES, playerActor, &scmHandleFloat1, &scmHandleFloat2, &scmHandleFloat3);
        float x = scmHandleFloat1, y = scmHandleFloat2, z = scmHandleFloat3;
        Log::file << "x = " << x << std::endl;
        Log::file << "y = " << y << std::endl;
        Log::file << "z = " << z << std::endl;

        int intX = (int)x;
        int intY = (int)y;
        Log::file << "scm_SHOW_TEXT_2NUMBERS_STYLED" << std::endl;
        sautils->ScriptCommand(&scm_SHOW_TEXT_2NUMBERS_STYLED, "MPFX2", intX, intY, 5000, 5);
        */

        //test 2
        int playerActor = 0;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "scm_GET_PLAYER_ACTOR" << std::endl;
        sautils->ScriptCommand(&scm_GET_PLAYER_ACTOR, 0, &playerActor);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "playerActor = " << playerActor << std::endl;

        float x = 0.0f, y = 0.0f, z = 0.0f;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "scm_GET_CHAR_COORDINATES" << std::endl;
        sautils->ScriptCommand(&scm_GET_CHAR_COORDINATES, playerActor, &x, &y, &z);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "x = " << x << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "y = " << y << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "z = " << z << std::endl;

        int intX = (int)x;
        int intY = (int)y;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "scm_SHOW_TEXT_2NUMBERS_STYLED" << std::endl;
        sautils->ScriptCommand(&scm_SHOW_TEXT_2NUMBERS_STYLED, "MPFX2", intX, intY, 5000, 5);

        Log::Level(LOG_LEVEL::LOG_BOTH) << "scm_IS_PLAYER_PLAYING" << std::endl;
        bool result = sautils->ScriptCommand(&scm_IS_PLAYER_PLAYING, 0);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "result = " << (result ? "1" : "0") << std::endl;

        if(result)
        {
            sautils->ScriptCommand(&scm_SHOW_TEXT_2NUMBERS_STYLED, "MPFX2", 1, 1, 5000, 5);
        } else {
            sautils->ScriptCommand(&scm_SHOW_TEXT_2NUMBERS_STYLED, "MPFX2", 0, 0, 5000, 5);
        }
    };

    auto button_test2 = window->AddButton(23, 2, 0);
    button_test2->onClick = []()
    {
        Remove();

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0.0f, y = 0.0f, z = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &x, &y, &z);

        int intX = (int)x;
        int intY = (int)y;
        CleoFunctions::SHOW_TEXT_2NUMBERS_STYLED("MPFX2", intX, intY, 3000, 5);

        int randomChar = CleoFunctions::GET_RANDOM_CHAR_IN_SPHERE(x, y, z, 10.0f, true, true, true);

        if(randomChar == -1)
        {
            CleoFunctions::SHOW_TEXT_2NUMBERS_STYLED("MPFX1", randomChar, 0, 3000, 5);
        } else {
            CleoFunctions::WAIT(2000, [randomChar]() {
                CleoFunctions::SHOW_TEXT_2NUMBERS_STYLED("MPFX1", randomChar, 0, 3000, 5);

                CleoFunctions::ADD_BLIP_FOR_CHAR(randomChar);
            });
        }
    };
    
    auto button_test4 = window->AddButton(23, 4, 0);
    button_test4->onClick = []()
    {
        Remove();

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0, y = 0, z = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 3.0f, 0, &x, &y, &z);

        CleoFunctions::CREATE_ACTOR_PEDTYPE(20, 284, x, y, z);
    };

    auto button_test5 = window->AddButton(23, 5, 0);
    button_test5->onClick = []()
    {
        Remove();

        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0, y = 0, z = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 10, 0, &x, &y, &z);

        auto car = CleoFunctions::CREATE_CAR_AT(523, x, y, z);

        int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 4, 284);
    };

    auto testWidgetId = window->AddIntRange(23, &Mod::m_TestWidgetId, 0, 200, 1);
    testWidgetId->holdToChange = false;

    /*
    //RADIO POSITION
    auto button_position = window->AddButton(9, 0, 0);
    button_position->onClick = [window]() {
        Menu::AddPosition2DWindow(window, &m_Position, -1000.0f, 1000.0f, 0.5f, []() {});
    };
    */
}

void WindowRadio::ToggleRadio(bool enabled)
{
    if(enabled == m_Enabled) return;

    m_Enabled = enabled;
    m_CurrentChannel = 0;
    m_CurrentFrequency = 0;
    m_ChangingChannels = true;

    if(enabled)
    {
        auto window = m_WindowRadio = Menu::AddWindow(6);
        window->position = CVector2D(0, 0);
        
        auto left_button = m_LeftButton = window->AddFloatingButton(4, 0, 0, CVector2D(0, 0), CVector2D(50, 50));
        left_button->visible = false;

        auto right_button = m_RightButton = window->AddFloatingButton(5, 0, 0, CVector2D(0, 0), CVector2D(50, 50));
        right_button->visible = false;

        auto ok_button = m_OkButton = window->AddFloatingButton(20, 0, 0, CVector2D(0, 0), CVector2D(50, 50), CRGBA(0, 180, 20));

        auto back_button = m_BackButton = window->AddFloatingButton(10, 0, 0, CVector2D(0, 0), CVector2D(50, 50), CRGBA(240, 90, 90));
    } else {
        m_WindowRadio->RemoveThisWindow();
        m_WindowRadio = NULL;
        m_LeftButton = NULL;
        m_RightButton = NULL;
        m_OkButton = NULL;
        m_BackButton = NULL;
    }

    if(enabled)
    {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        auto inCar = CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor);

        if(!inCar)
        {   
            if(radioObject != 0) CleoFunctions::DESTROY_OBJECT(radioObject);

            radioObject = CleoFunctions::CREATE_OBJECT(321, 0, 0, 0);
            CleoFunctions::ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION(playerActor, radioObject, 0, 0, 0, 6, 16, "PHONE_TALK", "PED", 1);
            //CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "PHONE_TALK", "PED", 4.0f, false, false, false, false, 1);
        }
    }
}

void WindowRadio::ToggleRadioOff(bool keepAnimation)
{
    ToggleRadio(false);

    int time = keepAnimation ? 2000 : 0;

    CleoFunctions::WAIT(time, []{
        if(m_Enabled) return;

        if(radioObject != 0) CleoFunctions::DESTROY_OBJECT(radioObject);
        radioObject = 0;
    });
}

void WindowRadio::Update(int dt)
{
    if(!m_Enabled) return;

    unsigned char alpha = m_TransparentButtons ? m_TransparentButtonAlpha : 255;

    if(m_LeftButton)
    {
        m_LeftButton->position = CVector2D(m_Position.x - 50, m_Position.y + 160);
        m_LeftButton->visible = false;

        m_LeftButton->box->color.a = alpha;
    }

    if(m_RightButton)
    {
        m_RightButton->position = CVector2D(m_Position.x + 110, m_Position.y + 160);
        m_RightButton->visible = false;

        m_RightButton->box->color.a = alpha;
    }

    if(m_OkButton)
    {
        m_OkButton->position = CVector2D(m_Position.x + 200, m_Position.y + 160);

        m_OkButton->box->color.a = alpha;
    }

    if(m_BackButton)
    {
        m_BackButton->position = CVector2D(m_Position.x + 260, m_Position.y + 160);

        m_BackButton->box->color.a = alpha;
    }

    //

    auto channel = m_Channels[m_CurrentChannel];

    // ----------------------------

    if(m_ChangingChannels == true && m_CurrentChannel > 0)
    {
        m_LeftButton->visible = true;

        if(m_LeftButton->hasPressedThisFrame) //-
        { 
            m_CurrentChannel--;
            CleoFunctions::PLAY_SOUND(0, 0, 0, 1052);
        }
    }

    if(m_ChangingChannels == false && m_CurrentFrequency > 0)
    {
        m_LeftButton->visible = true;

        if(m_LeftButton->hasPressedThisFrame) //-
        {
            m_CurrentFrequency--;
            CleoFunctions::PLAY_SOUND(0, 0, 0, 1052);
        }
    }
    
    // ----------------------------

    if(m_ChangingChannels == true && m_CurrentChannel < m_Channels.size() - 1)
    {
        m_RightButton->visible = true;

        if(m_RightButton->hasPressedThisFrame) //+
        {
            m_CurrentChannel++;
            CleoFunctions::PLAY_SOUND(0, 0, 0, 1052);
        }
    }

    if(m_ChangingChannels == false && m_CurrentFrequency < channel.frequencies.size() - 1)
    {
        m_RightButton->visible = true;

        if(m_RightButton->hasPressedThisFrame) //+
        {
           m_CurrentFrequency++;
           CleoFunctions::PLAY_SOUND(0, 0, 0, 1052);
        }
    }

    // ----------------------------
    
    if(m_OkButton)
    {
        if(m_OkButton->hasPressedThisFrame) //DEAL
        {
            CleoFunctions::PLAY_SOUND(0, 0, 0, 1052);

            if(m_ChangingChannels)
            {
                if(m_Channels[m_CurrentChannel].frequencies.size() == 0)
                {
                    SelectFrequency(m_CurrentChannel + 1, -1);
                } else {
                    m_ChangingChannels = false;
                    m_CurrentFrequency = 0;
                }

            } else {
                SelectFrequency(m_CurrentChannel + 1, m_CurrentFrequency + 1);
            }
        }
    }

    if(m_BackButton)
    {
        if(m_BackButton->hasPressedThisFrame) //EXIT
        {
            CleoFunctions::PLAY_SOUND(0, 0, 0, 1052);

            if(m_ChangingChannels)
            {
                ToggleRadioOff(false);
            } else {
                m_ChangingChannels = true;
            }
        }
    }
}

void WindowRadio::Draw()
{
    if(!m_Enabled) return;

    auto channel = m_Channels[m_CurrentChannel];
    
    auto textureId = channel.textureId;
    if(!m_ChangingChannels)
    {
        auto frequency = channel.frequencies[m_CurrentFrequency];
        textureId = frequency.textureId;
    }

    Draw::DrawSprite(textureId, CVector2D(m_Position.x, m_Position.y), CVector2D(150, 300), CRGBA(255, 255, 255));

    Draw::DrawGxtText(172, 0, 0, CVector2D(m_Position.x + 110, m_Position.y + 220), CRGBA(255, 255, 255), eTextAlign::ALIGN_LEFT);
}

void WindowRadio::SelectFrequency(int channelId, int frequencyId)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "SelectFrequency " << channelId << " : " << frequencyId << std::endl;

    if(channelId == 1)
    {
        bool hasCriminals = Callouts::GetCriminals().size() > 0;

        switch (frequencyId)
        {
        case 1: //VIATURA LS
            if(hasCriminals)
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio
                Backup::CallBackupCar(&Backup::m_DataBackupVehicles[0]);

                ToggleRadioOff(true);
            } else {
                ToggleRadioOff(false);
            }
            break;
        case 2: //ROCAM
            if(hasCriminals)
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX78", 0, 0, 0, 3000, 1); //apoio
                Backup::CallBackupCar(&Backup::m_DataBackupVehicles[1]);

                ToggleRadioOff(true);
            } else {
                ToggleRadioOff(false);
            }
            break;
        case 3: //HELICOPTERO
            if(hasCriminals)
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX88", 0, 0, 0, 3000, 1); //apoio
                Backup::CallBackupHeli();

                ToggleRadioOff(true);
            } else {
                ToggleRadioOff(false);
            }
        default:
            break;
        }
    }

    if(channelId == 2)
    {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        auto position = Mod::GetPedPositionWithOffset(playerActor, CVector(0, 100, 0));
        
        switch (frequencyId)
        {
        case 1: //OBSTACULOS
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX124", 0, 0, 0, 3000, 1); //apoio bloqueio

            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("roadblocks/REQUEST_ROADBLOCK_", false);

            Chase::AddRoadBlocks(position);

            ToggleRadioOff(true);
            break;
        case 2: //ESPINHOS
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX125", 0, 0, 0, 3000, 1); //apoio espinhos

            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("roadblocks/REQUEST_SPIKES_", false);

            Chase::AddSpikestrips(position);

            ToggleRadioOff(true);
            break;
        default:
            break;
        }
    }

    if(channelId == 3)
    {
        bool hasCriminals = Callouts::GetCriminals().size() > 0;

        switch (frequencyId)
        {
        case 1: //VIATURA SF
            if(hasCriminals)
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio
                Backup::CallBackupCar(&Backup::m_DataBackupVehicles[3]);

                ToggleRadioOff(true);
            } else {
                ToggleRadioOff(false);
            }
            break;
        case 2: //VIATURA LV
            if(hasCriminals)
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX77", 0, 0, 0, 3000, 1); //apoio
                Backup::CallBackupCar(&Backup::m_DataBackupVehicles[4]);

                ToggleRadioOff(true);
            } else {
                ToggleRadioOff(false);
            }
            break;
        case 3: //RANGER
            if(hasCriminals)
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX94", 0, 0, 0, 3000, 1); //apoio
                Backup::CallBackupCar(&Backup::m_DataBackupVehicles[5]);

                ToggleRadioOff(true);
            } else {
                ToggleRadioOff(false);
            }
            break;
        default:
            break;
        }
    }

    if(channelId == 4)
    {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        auto playerPosition = Mod::GetPedPosition(playerActor);
        auto closestVehicleHandle = Vehicles::GetClosestCar(playerPosition, 8.0f);
        auto closestVehicle = closestVehicleHandle > 0 ? Vehicles::GetVehicleByHandle(closestVehicleHandle) : NULL;

        switch (frequencyId)
        {
        case 1:
            SoundSystem::PlayHTAudio();
            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/REQUEST_AMBULANCE_", false);
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX120", 0, 0, 0, 3000, 1); //apoio ambulancia

            Ambulance::CallAmbulance(playerPosition);

            ToggleRadioOff(true);
            break;
        case 2:
            SoundSystem::PlayHTAudio();
            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/REQUEST_IML_", false);
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX121", 0, 0, 0, 3000, 1); //apoio IML

            Ambulance::CallIML(playerPosition);

            ToggleRadioOff(true);
            break;
        case 3:
            if(closestVehicle)
            {
                SoundSystem::PlayHTAudio();
                SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/REQUEST_TOW_TRUCK_", false);
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX110", 0, 0, 0, 3000, 1); //solicito guincho

                closestVehicle->AddBlip();
                Scorch::CallTowTruckToVehicle(closestVehicle);

                ToggleRadioOff(true);
            } else {
                ToggleRadioOff(false);
            }
            break;
        default:
            break;
        }
    }

    if(channelId == 5)
    {
        bool hasCriminals = Callouts::GetCriminals().size() > 0;

        switch (frequencyId)
        {
        case 1: //FBI
            if(hasCriminals)
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX94", 0, 0, 0, 3000, 1); //apoio
                Backup::CallBackupCar(&Backup::m_DataBackupVehicles[2]);

                ToggleRadioOff(true);
            } else {
                ToggleRadioOff(false);
            }
            break;
        case 2: //SWAT
            if(hasCriminals)
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX94", 0, 0, 0, 3000, 1); //apoio
                Backup::CallBackupCar(&Backup::m_DataBackupVehicles[7]);

                ToggleRadioOff(true);
            } else {
                ToggleRadioOff(false);
            }
            break;
        case 3: //EXERCITO
            /*
            if(hasCriminals)
            {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX94", 0, 0, 0, 3000, 1); //apoio
                Backup::CallBackupCar(&Backup::m_DataBackupVehicles[5]);
            }
            */
            ToggleRadioOff(false);

            break;
        default:
            break;
        }
    }

    if(channelId == 6)
    {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        auto playerPosition = Mod::GetPedPosition(playerActor);
        auto closestVehicleHandle = Vehicles::GetClosestCar(playerPosition, 8.0f);
        auto closestVehicle = closestVehicleHandle > 0 ? Vehicles::GetVehicleByHandle(closestVehicleHandle) : NULL;

        if(closestVehicle)
        {
            Pullover::CheckVehiclePlate(closestVehicle->hVehicle, [] () {});

            ToggleRadioOff(true);
        } else {
            ToggleRadioOff(false);
        }
    }

    if(channelId == 7)
    {
        WindowRadio::Create();

        ToggleRadioOff(false);
    }
}