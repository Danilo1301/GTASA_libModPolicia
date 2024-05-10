#include "CleoFunctions.h"

#include "isautils.h"
extern ISAUtils* sautils;


static DEFOPCODE(01C2, REMOVE_REFERENCES_TO_ACTOR, i); //01C2: remove_references_to_actor 2@
static DEFOPCODE(01B4, SET_PLAYER_CAN_MOVE, ib); //01B4: set_player $PLAYER_CHAR can_move 1 

static DEFOPCODE(0812, PERFORM_ANIMATION_AS_ACTOR, issfbbbbi); //0812: AS_actor 0@ perform_animation "handsup" IFP "PED" framedelta 4.0 loopA 0 lockX 0 lockY 0 lockF 1 time -1
static DEFOPCODE(01F5, GET_PLAYER_ACTOR, iv); //01F5: $PLAYER_ACTOR = get_player_actor $PLAYER_CHAR 
static DEFOPCODE(00A0, GET_CHAR_COORDINATES, ivvv);
static DEFOPCODE(036D, SHOW_TEXT_2NUMBERS_STYLED, siiii);
static DEFOPCODE(02FF, SHOW_TEXT_3NUMBERS, siiiii); //02FF: show_text_3numbers GXT 'MODPMV1' numbers 8@ 9@ 10@ time 2300 flag 1
static DEFOPCODE(073F, GET_RANDOM_CHAR_IN_SPHERE, ffffbbbv);
static DEFOPCODE(0187, ADD_BLIP_FOR_CHAR, iv);
static DEFOPCODE(0A51, IS_WIDGET_PRESSED, i);
static DEFOPCODE(0256, PLAYER_DEFINED, i); //0256: player $PLAYER_CHAR defined 
static DEFOPCODE(04EE, HAS_ANIMATION_LOADED, s); //04EE: animation "GANGS" loaded 
static DEFOPCODE(04ED, LOAD_ANIMATION, s); //04ED: load_animation "GANGS"


std::vector<WaitFunction*> CleoFunctions::m_WaitFunctions;

void CleoFunctions::Update(int dt)
{
    std::vector<WaitFunction*> completed;

    for(auto waitFunction : m_WaitFunctions)
    {
        waitFunction->timePassed += dt;
        if(waitFunction->timePassed >= waitFunction->time)
        {
            completed.push_back(waitFunction);
        }
    }

    for(auto waitFunction : completed) {
        waitFunction->callback();
        RemoveWaitFunction(waitFunction);
    }
}

WaitFunction* CleoFunctions::AddWaitFunction(int time, std::function<void()> callback)
{
    WaitFunction* waitFunction = new WaitFunction();
    waitFunction->time = time;
    waitFunction->callback = callback;
    m_WaitFunctions.push_back(waitFunction);
    return waitFunction;
}

void CleoFunctions::RemoveWaitFunction(WaitFunction* waitFunction)
{
    auto it = std::find(m_WaitFunctions.begin(), m_WaitFunctions.end(), waitFunction);
    if (it == m_WaitFunctions.end()) return;
    m_WaitFunctions.erase(it);
    delete waitFunction;
}

void CleoFunctions::WAIT(int time, std::function<void()> callback)
{
    AddWaitFunction(time, callback);
}

void CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(int _char)
{
    sautils->ScriptCommand(&scm_REMOVE_REFERENCES_TO_ACTOR, _char);
}

void CleoFunctions::SET_PLAYER_CAN_MOVE(int player, bool state)
{
    sautils->ScriptCommand(&scm_SET_PLAYER_CAN_MOVE, player, state);
}

void CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(int _char, const char* animationName, const char* animationFile, float frameDelta, bool loop, bool lockX, bool lockY, bool lockF, int time)
{
    sautils->ScriptCommand(&scm_PERFORM_ANIMATION_AS_ACTOR, _char, animationName, animationFile, frameDelta, loop, lockX, lockY, lockF, time);
}

int CleoFunctions::GET_PLAYER_ACTOR(int player)
{
    int playerActor = 0;
    sautils->ScriptCommand(&scm_GET_PLAYER_ACTOR, 0, &playerActor);
    return playerActor;
}

void CleoFunctions::GET_CHAR_COORDINATES(int _char, float* x, float* y, float* z)
{
    sautils->ScriptCommand(&scm_GET_CHAR_COORDINATES, _char, x, y, z);
}

void CleoFunctions::SHOW_TEXT_2NUMBERS_STYLED(const char* key, int num1, int num2, int duration, int style)
{
    sautils->ScriptCommand(&scm_SHOW_TEXT_2NUMBERS_STYLED, key, num1, num2, duration, style);
}

void CleoFunctions::SHOW_TEXT_3NUMBERS(const char* key, int num1, int num2, int num3, int duration, int style)
{
    sautils->ScriptCommand(&scm_SHOW_TEXT_3NUMBERS, key, num1, num2, num3, duration, style);
}

int CleoFunctions::GET_RANDOM_CHAR_IN_SPHERE(float x, float y, float z, float radius, bool civilian, bool gang, bool criminal)
{
    int _char = 0;
    sautils->ScriptCommand(&scm_GET_RANDOM_CHAR_IN_SPHERE, x, y, z, radius, civilian, gang, criminal, &_char);
    return _char;
}

int CleoFunctions::ADD_BLIP_FOR_CHAR(int _char)
{
    int blip = 0;
    sautils->ScriptCommand(&scm_ADD_BLIP_FOR_CHAR, _char, &blip);
    return blip;
}

bool CleoFunctions::IS_WIDGET_PRESSED(int widgetId)
{
    bool result = sautils->ScriptCommand(&scm_IS_WIDGET_PRESSED, widgetId);
    return result;
}

bool CleoFunctions::PLAYER_DEFINED(int player)
{
    bool result = sautils->ScriptCommand(&scm_PLAYER_DEFINED, 0);
    return result;
}

bool CleoFunctions::HAS_ANIMATION_LOADED(const char* animationFile)
{
    bool result = sautils->ScriptCommand(&scm_HAS_ANIMATION_LOADED, animationFile);
    return result;
}

void CleoFunctions::LOAD_ANIMATION(const char* animationFile)
{
    sautils->ScriptCommand(&scm_LOAD_ANIMATION, animationFile);
}

