#include "CleoFunctions.h"

#include "isautils.h"
extern ISAUtils* sautils;



static DEFOPCODE(009B, DESTROY_ACTOR, i); //009B: destroy_actor 4@ 
static DEFOPCODE(03BD, DESTROY_SPHERE, i); //03BD: destroy_sphere 7@
static DEFOPCODE(03BC, CREATE_SPHERE, ffffv); //03BC: 7@ = create_sphere_at 1536.1325 -1671.2093 13.3828 radius 3.0 
static DEFOPCODE(0631, PUT_ACTOR_IN_GROUP, ii); //0631: put_actor 4@ in_group 6@
static DEFOPCODE(0630, PUT_ACTOR_IN_GROUP_AS_LEADER, ii); //0630: put_actor $PLAYER_ACTOR in_group 4@ as_leader
static DEFOPCODE(07AF, GET_PLAYER_GROUP, iv); //07AF: 6@ = player $PLAYER_CHAR group

static DEFOPCODE(05CB, ENTER_CAR_AS_DRIVER_AS_ACTOR, iii); //05CB: AS_actor 21@ enter_car 0@ as_driver 20000 ms
static DEFOPCODE(0633, EXIT_CAR_AS_ACTOR, i); //0633: AS_actor 4@ exit_car
static DEFOPCODE(02D4, CAR_TURN_OFF_ENGINE, i); //02D4: car 3@ turn_off_engine 
static DEFOPCODE(046C, GET_DRIVER_OF_CAR, iv); //046C: 133@ = car 50@ driver 
static DEFOPCODE(0407, STORE_COORDS_FROM_CAR_WITH_OFFSET, ifffvvv); //0407: store_coords_to 128@ 138@ 148@ from_car 551@ with_offset -0.337 1.566 0.657 
static DEFOPCODE(04C4, STORE_COORDS_FROM_ACTOR_WITH_OFFSET, ifffvvv); //04C4: store_coords_to 4@ 5@ 6@ from_actor $PLAYER_ACTOR with_offset 1.0 2.0 0.0 
static DEFOPCODE(073E, GET_CAR_IN_SPHERE, ffffiv); //073E: get_car_in_sphere -1577.942 52.6333 16.3281 radius 4.0 model -1 handle_as $47 
static DEFOPCODE(00DF, IS_CHAR_IN_ANY_CAR, i); //00DF: actor $PLAYER_ACTOR driving 
static DEFOPCODE(01C2, REMOVE_REFERENCES_TO_ACTOR, i); //01C2: remove_references_to_actor 2@
static DEFOPCODE(01B4, SET_PLAYER_CAN_MOVE, ib); //01B4: set_player $PLAYER_CHAR can_move 1 
static DEFOPCODE(0812, PERFORM_ANIMATION_AS_ACTOR, issfbbbbi); //0812: AS_actor 0@ perform_animation "handsup" IFP "PED" framedelta 4.0 loopA 0 lockX 0 lockY 0 lockF 1 time -1
static DEFOPCODE(01F5, GET_PLAYER_ACTOR, iv); //01F5: $PLAYER_ACTOR = get_player_actor $PLAYER_CHAR 
static DEFOPCODE(00A0, GET_CHAR_COORDINATES, ivvv);
static DEFOPCODE(036D, SHOW_TEXT_2NUMBERS_STYLED, siiii);
static DEFOPCODE(02FF, SHOW_TEXT_3NUMBERS, siiiii); //02FF: show_text_3numbers GXT 'MODPMV1' numbers 8@ 9@ 10@ time 2300 flag 1
static DEFOPCODE(073F, GET_RANDOM_CHAR_IN_SPHERE, ffffbbbv);
static DEFOPCODE(0164, DISABLE_MARKER, i); //0164: disable_marker $482 
static DEFOPCODE(0186, ADD_BLIP_FOR_CAR, iv); //0186: $60 = create_marker_above_car $59 
static DEFOPCODE(0187, ADD_BLIP_FOR_CHAR, iv); //0187: 47@ = create_marker_above_actor 75@ 
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
        if(waitFunction->isTestFunction)
        {
            auto result = waitFunction->testFn();
            if(result) completed.push_back(waitFunction);
            continue;
        }

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

WaitFunction* CleoFunctions::AddWaitForFunction(std::function<bool()> testFn, std::function<void()> callback)
{
    WaitFunction* waitFunction = new WaitFunction();
    waitFunction->time = 0;
    waitFunction->callback = callback;
    waitFunction->isTestFunction = true;
    waitFunction->testFn = testFn;
    m_WaitFunctions.push_back(waitFunction);

    Update(0);

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

void CleoFunctions::DESTROY_ACTOR(int actor)
{
    sautils->ScriptCommand(&scm_DESTROY_ACTOR, actor);
}

void CleoFunctions::DESTROY_SPHERE(int sphere)
{
    sautils->ScriptCommand(&scm_DESTROY_SPHERE, sphere);
}

int CleoFunctions::CREATE_SPHERE(float x, float y, float z, float radius)
{
    int sphere = 0;
    sautils->ScriptCommand(&scm_CREATE_SPHERE, x, y, z, radius, &sphere);
    return sphere;
}

void CleoFunctions::PUT_ACTOR_IN_GROUP(int group, int _char)
{
    sautils->ScriptCommand(&scm_PUT_ACTOR_IN_GROUP, group, _char);
}

void CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(int group, int _char)
{
    sautils->ScriptCommand(&scm_PUT_ACTOR_IN_GROUP_AS_LEADER, group, _char);
}

int CleoFunctions::GET_PLAYER_GROUP(int player)
{
    int group = 0;
    sautils->ScriptCommand(&scm_GET_PLAYER_GROUP, player, &group);
    return group;
}

void CleoFunctions::ENTER_CAR_AS_DRIVER_AS_ACTOR(int _char, int vehicle, int time)
{
    sautils->ScriptCommand(&scm_ENTER_CAR_AS_DRIVER_AS_ACTOR, _char, vehicle, time);
}

void CleoFunctions::EXIT_CAR_AS_ACTOR(int _actor)
{
    sautils->ScriptCommand(&scm_EXIT_CAR_AS_ACTOR, _actor);
}

void CleoFunctions::CAR_TURN_OFF_ENGINE(int car)
{
    sautils->ScriptCommand(&scm_CAR_TURN_OFF_ENGINE, car);
}

int CleoFunctions::GET_DRIVER_OF_CAR(int car)
{
    int _char = 0;
    sautils->ScriptCommand(&scm_GET_DRIVER_OF_CAR, car, &_char);
    return _char;
}

void CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(int car, float offsetX, float offsetY, float offsetZ, float* x, float* y, float* z)
{
    sautils->ScriptCommand(&scm_STORE_COORDS_FROM_CAR_WITH_OFFSET, car, offsetX, offsetY, offsetZ, x, y, z);
}

void CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(int _char, float offsetX, float offsetY, float offsetZ, float* x, float* y, float* z)
{
    sautils->ScriptCommand(&scm_STORE_COORDS_FROM_ACTOR_WITH_OFFSET, _char, offsetX, offsetY, offsetZ, x, y, z);
}

/*
void CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(float* x, float* y, float* z, int _char, float offsetX, float offsetY, float offsetZ)
{
    sautils->ScriptCommand(&scm_STORE_COORDS_FROM_ACTOR_WITH_OFFSET, x, y, z, _char, offsetX, offsetY, offsetZ);
}
*/

int CleoFunctions::GET_CAR_IN_SPHERE(float x, float y, float z, float radius, int modelVehicle)
{
    int car = 0;
    sautils->ScriptCommand(&scm_GET_CAR_IN_SPHERE, x, y, z, radius, modelVehicle, &car);
    return car;
}

bool CleoFunctions::IS_CHAR_IN_ANY_CAR(int _char)
{
    bool result = sautils->ScriptCommand(&scm_IS_CHAR_IN_ANY_CAR, _char);
    return result;
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

void CleoFunctions::DISABLE_MARKER(int blip)
{
    sautils->ScriptCommand(&scm_DISABLE_MARKER, blip);
}

int CleoFunctions::ADD_BLIP_FOR_CAR(int car)
{
    int blip = 0;
    sautils->ScriptCommand(&scm_ADD_BLIP_FOR_CAR, car, &blip);
    return blip;
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

