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
#include "windows/WindowDocument.h"

#include "windows/WindowTest.h"

extern CVector2D *m_vecCachedPos;

const char* Mod::m_Version = "0.2.0";
unsigned int Mod::m_TimePassed = 0;

bool loadedAnimations = false;

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

    Peds::Update(dt);

    Pullover::Update(dt);

    CleoFunctions::Update(dt);

    WindowDocument::Draw();

    Menu::Update(dt);
    
    Menu::Draw();

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
        if(!loadedAnimations)
        {
            Log::file << "Checking for animations..." << std::endl;

            if(CleoFunctions::HAS_ANIMATION_LOADED("POLICE") && CleoFunctions::HAS_ANIMATION_LOADED("GANGS"))
            {
                loadedAnimations = true;

                Log::file << "Animations are already loaded!" << std::endl;
            } else {
                Log::file << "Loading animations..." << std::endl;

                CleoFunctions::LOAD_ANIMATION("GANGS");
                CleoFunctions::LOAD_ANIMATION("POLICE");
            }
        }
    }
    
    if(Widgets::IsWidgetPressed(7))
    {
        //Log::file << "[Mod] Widget 7 is pressed" << std::endl;
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

   //Log::file << "Mod::Update end" << std::endl;
}

void Mod::Load()
{
    InventoryItems::Init();
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
    Mod::ProcessTestMenuButtons(dt);

    //test menu, 5 and 6
    if (Input::GetTouchIdState(6) && Input::GetTouchIdState(5))
    {
        if (Input::GetTouchIdPressTime(6) > 500)
        {
            
        }
    }
}


void Mod::ProcessTestMenuButtons(int dt)
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