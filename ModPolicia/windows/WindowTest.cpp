#include "WindowTest.h"

#include "../Log.h"
#include "../CleoFunctions.h"
#include "../Pullover.h"
#include "../Peds.h"
#include "../Chase.h"

#include "WindowDocument.h"

Window* WindowTest::m_Window = NULL;

#include "isautils.h"
extern ISAUtils* sautils;
int scmHandle1, scmHandle2;
float scmHandleFloat1, scmHandleFloat2, scmHandleFloat3;
static DEFOPCODE(01F5, GET_PLAYER_ACTOR, iv); //01F5: $PLAYER_ACTOR = get_player_actor $PLAYER_CHAR 
static DEFOPCODE(00A0, GET_CHAR_COORDINATES, ivvv);
static DEFOPCODE(036D, SHOW_TEXT_2NUMBERS_STYLED, siiii);
static DEFOPCODE(0256, IS_PLAYER_PLAYING, i); //0256:  player $PLAYER_CHAR defined 

void WindowTest::Create()
{
	if (m_Window) return;

    auto window = m_Window = Menu::AddWindow(6);
    window->position = CVector2D(200, 200); //80, 200
    window->showPageControls = true;

    auto button_position = window->AddButton(9, 1, 0);
    button_position->onClick = [window]() {
        Menu::AddPosition2DWindow(window, &WindowDocument::m_TestPosition, -1000.0f, 1000.0f, 0.5f, []() {});
    };

    window->AddCheckbox(29, &Menu::m_DrawCursor);

    auto button_test = window->AddButton(23, 1, 0);
    button_test->onClick = []()
    {
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
    
    auto button_test3 = window->AddButton(23, 3, 0);
    button_test3->onClick = []()
    {
        Chase::EndChase();
    };

    auto button_test4 = window->AddButton(23, 4, 0);
    button_test4->onClick = []()
    {
        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0, y = 0, z = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 3.0f, 0, &x, &y, &z);

        CleoFunctions::CREATE_ACTOR_PEDTYPE(20, 284, x, y, z);
    };

    auto button_test5 = window->AddButton(23, 5, 0);
    button_test5->onClick = []()
    {
        int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float x = 0, y = 0, z = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 10, 0, &x, &y, &z);

        auto car = CleoFunctions::CREATE_CAR_AT(523, x, y, z);

        int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 4, 284);
    };


    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        WindowTest::Remove();
    };
}

void WindowTest::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}