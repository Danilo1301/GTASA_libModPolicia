#include "CleoFunctions.h"

#include "Log.h"

#include "isautils.h"
extern ISAUtils* sautils;

static DEFOPCODE(02EB, RESTORE_CAMERA_WITH_JUMPCUT, ); //02EB: restore_camera_with_jumpcut

static DEFOPCODE(015F, SET_CAMERA_POSITION, ffffff); //015F: set_camera_position 1@ 2@ 3@ rotation 0.0 0.0 0.0
static DEFOPCODE(0160, SET_CAMERA_POINT_AT, fffi); //0160: set_camera_point_at 1@ 2@ 3@ switchstyle 1
static DEFOPCODE(01BC, PUT_OBJECT_AT, ifff); //01BC: put_object 0@ at $72 $73 $74 
static DEFOPCODE(07A3, GOTO_CHAR_AIMING, iiff); //07A3: AS_actor $GUNNER run_to_and_follow_actor $VICTIM wait_radius_between 0.5 and 1.0
static DEFOPCODE(01B9, SET_ACTOR_ARMED_WEAPON, ii); //01B9: set_actor 2@ armed_weapon_to 0 
static DEFOPCODE(0635, AIM_AT_ACTOR, iii); //0635: AS_actor -1 aim_at_actor $PLAYER_ACTOR 2000 ms 
static DEFOPCODE(03A3, ACTOR_MALE, i); //03A3: actor 0@ male 
static DEFOPCODE(0174, GET_CAR_Z_ANGLE, iv); //0174: $1339 = car $1314 Z_angle 
static DEFOPCODE(0470, GET_CURRENT_WEAPON, iv); //0470: 84@ = actor $PLAYER_ACTOR current_weapon 
static DEFOPCODE(04B8, GET_WEAPON_DATA_FROM_ACTOR, iivvv); //04B8: get_weapon_data_from_actor $PLAYER_ACTOR slot 2 weapon 404@ ammo 405@ model 405@ 
static DEFOPCODE(070A, ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION, iifffiissi); //070A: AS_actor $PLAYER_ACTOR attach_to_object 38@ offset 0.0 0.0 0.0 on_bone 6 16 perform_animation "PHONE_TALK" IFP_file "PED" time 1 
static DEFOPCODE(018C, PLAY_SOUND, fffi); //018C: play_sound 1052 at 0.0 0.0 0.0
static DEFOPCODE(0750, SET_OBJECT_VISIBILITY, ib); //0750: set_object $OBJ visibility 0
static DEFOPCODE(0109, ADD_MONEY, ii); //0109
static DEFOPCODE(0727, SET_HELI_BEHAVIOR_TO_POLICE_HELI_AND_FOLLOW, iiif); //0727: set_heli $HELI behavior_to_police_heli_and_follow_actor -1 follow_car $CAR radius 15.0
static DEFOPCODE(0173, SET_ACTOR_ANGLE_TO, if); //0173: set_actor $PLAYER_ACTOR Z_angle_to 90.0 
static DEFOPCODE(0464, PUT_ACTOR_INTO_TURRET_ON_CAR, iifffifi); //0464: put_actor $PED into_turret_on_car $CAR at_car_offset 0.0 0.0 0.0 position 0 shooting_angle_limit 0.0 with_weapon 0
static DEFOPCODE(0465, REMOVE_ACTOR_FROM_TURRET_MODE, i); //0465: remove_actor $PED from_turret_mode
static DEFOPCODE(0619, ENABLE_ACTOR_COLLISION_DETECTION, ib); //0619: enable_actor $PED2 collision_detection 0
static DEFOPCODE(0382, SET_OBJECT_COLLISION_DETECTION, ib); //0382: set_object $OBJ collision_detection 0
static DEFOPCODE(04F4, PUT_ACTOR_INTO_TURRET_ON_OBJECT, iifffifi); //04F4: put_actor $PED2 into_turret_on_object $OBJ offset_from_object_origin 0.0 0.0 0.0 orientation 0 both_side_angle_limit 360.0 lock_weapon 0 
static DEFOPCODE(069B, ATTACH_OBJECT_TO_ACTOR, iiffffff); //069B: attach_object $OBJ to_actor $PLAYER_ACTOR with_offset 0.0 1.0 0.0 rotation 0.0 0.0 0.0
static DEFOPCODE(0226, ACTOR_HEALTH, iv); //0226: $7826 = actor 173@ health
static DEFOPCODE(010D, SET_PLAYER_WANTED_LEVEL, ii); //010D: set_player $PLAYER_CHAR wanted_level_to 0
static DEFOPCODE(0215, DESTROY_PICKUP, i); //0215: destroy_pickup $3071[0]
static DEFOPCODE(0213, CREATE_PICKUP, iifffv); //0213: $PICKUP = create_pickup 1210 type 6 at 1@ 2@ 3@
static DEFOPCODE(0223, SET_ACTOR_HEALTH, ii); //0223: set_actor 2@ health_to 500 
static DEFOPCODE(0743, HELI_FLY_TO, ifffff); //0743: heli 45@ fly_to -2244.48 129.14 34.56 altitude 0.0 0.0 
static DEFOPCODE(0633, AS_ACTOR_EXIT_CAR, i); //0633: AS_actor 58@ exit_car 
static DEFOPCODE(02E3, CAR_SPEED, iv); //02E3: 196@ = car 67@ speed 
static DEFOPCODE(0811, ACTOR_USED_CAR, iv); //0811: $47 = actor $PLAYER_ACTOR used_car 
static DEFOPCODE(00DF, ACTOR_DRIVING, i); //00DF: actor $PLAYER_ACTOR driving 
static DEFOPCODE(0713, ACTOR_DRIVEBY, iiiffffibi); //0713: actor $PED2 driveby_actor $COP car -1 point 0.0 0.0 0.0 radius 100.0 4 1 firing_rate 90
static DEFOPCODE(0431, CAR_PASSENGER_SEAT_FREE, ii); //0431: car $47 passenger_seat_free 0
static DEFOPCODE(0432, GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT, iiv); //0432: 19@ = get_actor_handle_from_car $47 passenger_seat 0 
static DEFOPCODE(01EA, CAR_MAX_PASSENGERS, iv); //01EA: 68@ = car 67@ max_passengers 
static DEFOPCODE(02A0, ACTOR_STOPPED, i); //02A0: actor $PLAYER_ACTOR stopped 
static DEFOPCODE(0248, MODEL_AVAILABLE, i); //0248: model 596 available
static DEFOPCODE(0247, LOAD_MODEL, i); //0247: load_model 110
static DEFOPCODE(038B, LOAD_REQUESTED_MODELS, ); //038B: load_requested_models
static DEFOPCODE(0108, DESTROY_OBJECT, i); //0108: destroy_object $1173 
static DEFOPCODE(04FE, DEFLATE_TIRE_ON_CAR, ii); //04FE: deflate_tire 0 on_car $PLAYER_CAR
static DEFOPCODE(0177, SET_OBJECT_Z_ANGLE, if); //0177: set_object $OBJ1 Z_angle_to $ANGLE
static DEFOPCODE(0107, CREATE_OBJECT, ifffv); //0107: $OBJ1 = create_object 1459 at 4@ 5@ 6@
static DEFOPCODE(02CE, GROUND_Z_AT, fffv); //02CE: $GROUND = ground_z_at 4@ 5@ 6@
static DEFOPCODE(00AB, PUT_CAR_AT, ifff); //00AB: put_car $CAR1 at 4@ 5@ 6@
static DEFOPCODE(0175, SET_CAR_Z_ANGLE, if); //0175: set_car $CAR1 Z_angle_to $ANGLE
static DEFOPCODE(0172, ACTOR_Z_ANGLE, iv); //0172: $ANGLE = actor $PLAYER_ACTOR Z_angle 
static DEFOPCODE(0668, ROTATE_AND_SHOOT, ifffi); //0668: AS_actor $PED rotate_and_shoot_at 1@ 2@ 3@ 10000 ms
static DEFOPCODE(0665, GET_ACTOR_MODEL, iv); //0665: get_actor 0@ model_to 7@
static DEFOPCODE(05D9, AS_ACTOR_RUN_TO_ACTOR, iiif); //05D9: AS_actor $PED2 run_to_actor $PLAYER_ACTOR timelimit 5000 stop_within_radius 1.0
static DEFOPCODE(01F0, SET_MAX_WANTED_LEVEL_TO, i); //01F0: set_max_wanted_level_to 0 
static DEFOPCODE(01F7, SET_PLAYER_IGNORED_BY_COPS, ib); //01F7: set_player $PLAYER_CHAR ignored_by_cops 1
static DEFOPCODE(0893, PUT_TRAILER_ON_CAB, ii); //0893: put_trailer $CAR on_cab $TOW
static DEFOPCODE(05D1, AS_ACTOR_DRIVE_CAR_TO, iiffffiii); //05D1: AS_actor $PED drive_car $CAR to 1@ 2@ 3@ speed 15.0 0 model #NULL 0
static DEFOPCODE(0687, CLEAR_ACTOR_TASK, i); //0687: clear_actor $PLAYER_ACTOR task 
static DEFOPCODE(09C7, CHANGE_PLAYER_MODEL_TO, ii); //09C7: change_player 0 model_to 280
static DEFOPCODE(0441, GET_CAR_MODEL, iv); //0441: 7@ = car $47 model 
static DEFOPCODE(01C8, CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT, iiiiv); //01C8: $P2 = create_actor_pedtype 23 model 280 in_car $POLICE_CAR passenger_seat 0 
static DEFOPCODE(01B2, GIVE_ACTOR_WEAPON, iii); //01B2: give_actor 3@ weapon 22 ammo 10000
static DEFOPCODE(03E5, SHOW_TEXT_BOX, s); //03E5: show_text_box 'MPFX86'
static DEFOPCODE(0726, HELI_FOLLOW, iiif); //0726: heli 3@ follow_actor -1 follow_car 5@ radius 15.0
static DEFOPCODE(0918, SET_CAR_ENGINE_OPERATION, ib); //0918: set_car 3@ engine_operation 1 
static DEFOPCODE(0825, SET_HELICOPTER_INSTANT_ROTOR_START, i); //0825: set_helicopter 3@ instant_rotor_start
static DEFOPCODE(016A, FADE, ii); //016A: fade 1 time 500
static DEFOPCODE(00A6, DESTROY_CAR, i); //00A6: destroy_car 7@
static DEFOPCODE(020A, SET_CAR_DOOR_STATUS, ii); //020A: set_car 0@ door_status_to 1 
static DEFOPCODE(05CA, ACTOR_ENTER_CAR_PASSENGER_SEAT, iiii); //05CA: AS_actor 3@ enter_car 7@ passenger_seat 1 time 10000
static DEFOPCODE(00A7, CAR_DRIVE_TO, ifff); //00A7: car 7@ drive_to 0@ 1@ 2@
static DEFOPCODE(0519, FREEZE_CAR_POSITION, ib); //0519: set_car 7@ locked 0 
static DEFOPCODE(0165, SET_MARKER_COLOR_TO, ii); //0165: set_marker 9@ color_to 2
static DEFOPCODE(0114, SET_ACTOR_WEAPON_AND_AMMO, iii); //0114: set_actor $PLAYER_ACTOR weapon 38 add_ammo 200000 
static DEFOPCODE(0397, ENABLE_CAR_SIREN, ib); //0397: enable_car 6@ siren 1
static DEFOPCODE(0118, ACTOR_DEAD, i); //0118: actor 0@ dead 
static DEFOPCODE(056D, ACTOR_DEFINED, i); //056D:   actor 0@ defined 
static DEFOPCODE(09B6, SET_ACTOR_WANTED_BY_POLICE, ib); //09B6: set_actor 6@ wanted_by_police 1 
static DEFOPCODE(05E2, KILL_ACTOR, ii); //05E2: AS_actor 6@ kill_actor 7@ 
static DEFOPCODE(05DD, FLEE_FROM_ACTOR, iifi); //05DD: AS_actor 7@ flee_from_actor 6@ from_origin_radius 1000.0 timelimit -1
static DEFOPCODE(02C0, STORE_PED_PATH_COORDS_CLOSEST_TO, fffvvv); //02C0: store_to 3@ 4@ 5@ ped_path_coords_closest_to 0@ 1@ 2@
static DEFOPCODE(009A, CREATE_ACTOR_PEDTYPE, iifffv); //009A: 6@ = create_actor_pedtype 20 model #DNFYLC at 3@ 4@ 5@
static DEFOPCODE(0168, SET_MARKER_SIZE, ii); //0168: set_marker 9@ size 3
static DEFOPCODE(0457, PLAYER_AIMING_AT_ACTOR, ii); //0457: player $PLAYER_CHAR aiming_at_actor 0@
static DEFOPCODE(07F8, CAR_FOLLOR_CAR, iif); //07F8: car 6@ follow_car 8@ radius 8.0
static DEFOPCODE(0129, CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT, iiiv); //0129: 10@ = create_actor_pedtype 23 model #LAPD1 in_car 6@ driverseat
static DEFOPCODE(00A5, CREATE_CAR_AT, ifffv); //00A5: 6@ = create_car #COPCARLA at 3@ 4@ 5@
static DEFOPCODE(04D3, GET_NEAREST_CAR_PATH_COORDS_FROM, fffivvv); //04D3: get_nearest_car_path_coords_from 0@ 1@ 2@ type 2 store_to 3@ 4@ 5@
static DEFOPCODE(0167, CREATE_MARKER_AT, fffiiv); //0167: 7@ = create_marker_at 0@ 1@ 2@ color 0 flag 3
static DEFOPCODE(0611, ACTOR_PERFORMING_ANIMATION, is); //0611: actor 2@ performing_animation "LRGIRL_IDLE_TO_L0" 
static DEFOPCODE(056E, CAR_DEFINED, i); //056E: car 3@ defined
static DEFOPCODE(00AE, SET_CAR_TRAFFIC_BEHAVIOUR, ii); //00AE: set_car 3@ traffic_behaviour_to 2
static DEFOPCODE(00AD, SET_CAR_MAX_SPEED, if); //00AD: set_car 3@ max_speed_to 50.0  
static DEFOPCODE(00A8, SET_CAR_TO_PSYCHO_DRIVER, i); //00A8: set_car 52@ to_psycho_driver 
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
static DEFOPCODE(01C3, REMOVE_REFERENCES_TO_CAR, i); //01C3: remove_references_to_car $1144 
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
static DEFOPCODE(0A51, IS_WIDGET_PRESSED, i); //0A51: is_widget_pressed 37 
static DEFOPCODE(0256, PLAYER_DEFINED, i); //0256: player $PLAYER_CHAR defined 
static DEFOPCODE(04EE, HAS_ANIMATION_LOADED, s); //04EE: animation "GANGS" loaded 
static DEFOPCODE(04ED, LOAD_ANIMATION, s); //04ED: load_animation "GANGS"

std::vector<WaitFunction*> CleoFunctions::m_WaitFunctions;

void CleoFunctions::Update(int dt)
{
    for(auto waitFunction : m_WaitFunctions)
    {
        if(waitFunction->isConditionFunction)
        {
            waitFunction->conditionFn([waitFunction] () {
                waitFunction->state = WAIT_FN_STATE::WAIT_FN_COMPLETED;
            }, [waitFunction] () {
                waitFunction->state = WAIT_FN_STATE::WAIT_FN_CANCELLED;
            });
            continue;
        }

        if(waitFunction->isTestFunction)
        {
            auto result = waitFunction->testFn();
            if(result) waitFunction->state = WAIT_FN_STATE::WAIT_FN_COMPLETED;
            continue;
        }

        waitFunction->timePassed += dt;
        if(waitFunction->timePassed >= waitFunction->time)
        {
            waitFunction->state = WAIT_FN_STATE::WAIT_FN_COMPLETED;
        }
    }

    std::vector<WaitFunction*> toRemove;

    for(auto waitFunction : m_WaitFunctions)
    {
        if(waitFunction->state == WAIT_FN_STATE::WAIT_FN_COMPLETED)
        {
            waitFunction->onComplete();
            toRemove.push_back(waitFunction);
        }

        if(waitFunction->state == WAIT_FN_STATE::WAIT_FN_CANCELLED)
        {
            waitFunction->onCancel();
            toRemove.push_back(waitFunction);
        }
    }

    for(auto waitFunction : toRemove)
    {
        RemoveWaitFunction(waitFunction);
    }
}

WaitFunction* CleoFunctions::AddWaitFunction(int time, std::function<void()> callback)
{
    WaitFunction* waitFunction = new WaitFunction();
    waitFunction->time = time;
    waitFunction->onComplete = callback;
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

WaitFunction* CleoFunctions::AddWaitForFunction(std::function<bool()> testFn, std::function<void()> callback)
{
    WaitFunction* waitFunction = new WaitFunction();
    waitFunction->time = 0;
    waitFunction->onComplete = callback;
    waitFunction->isTestFunction = true;
    waitFunction->testFn = testFn;
    m_WaitFunctions.push_back(waitFunction);

    return waitFunction;
}

WaitFunction* CleoFunctions::AddCondition(std::function<void(std::function<void()>, std::function<void()>)> fn, std::function<void()> onComplete, std::function<void()> onCancel)
{
    WaitFunction* waitFunction = new WaitFunction();
    waitFunction->time = 0;
    waitFunction->onComplete = onComplete;
    waitFunction->onCancel = onCancel;
    waitFunction->isConditionFunction = true;
    waitFunction->conditionFn = fn;
    m_WaitFunctions.push_back(waitFunction);

    return waitFunction;
}

void CleoFunctions::WAIT(int time, std::function<void()> callback)
{
    AddWaitFunction(time, callback);
}

void CleoFunctions::RESTORE_CAMERA_WITH_JUMPCUT()
{
    sautils->ScriptCommand(&scm_RESTORE_CAMERA_WITH_JUMPCUT);
}

void CleoFunctions::SET_CAMERA_POSITION(float x, float y, float z, float rotX, float rotY, float rotZ)
{
    sautils->ScriptCommand(&scm_SET_CAMERA_POSITION, x, y, z, rotX, rotY, rotZ);
}

void CleoFunctions::SET_CAMERA_POINT_AT(float x, float y, float z, int switchStyle)
{
    sautils->ScriptCommand(&scm_SET_CAMERA_POINT_AT, x, y, z, switchStyle);
}

void CleoFunctions::PUT_OBJECT_AT(int object, float x, float y, float z)
{
    sautils->ScriptCommand(&scm_PUT_OBJECT_AT, object, x, y, z);
}

void CleoFunctions::GOTO_CHAR_AIMING(int handle, int target, float radiusFrom, float radiusTo)
{
    sautils->ScriptCommand(&scm_GOTO_CHAR_AIMING, handle, target, radiusFrom, radiusTo);
}

void CleoFunctions::SET_ACTOR_ARMED_WEAPON(int _char, int weaponType)
{
    sautils->ScriptCommand(&scm_SET_ACTOR_ARMED_WEAPON, _char, weaponType);
}

void CleoFunctions::AIM_AT_ACTOR(int _char, int target, int time)
{
    sautils->ScriptCommand(&scm_AIM_AT_ACTOR, _char, target, time);
}

bool CleoFunctions::ACTOR_MALE(int _char)
{
    bool result = false;
    result = sautils->ScriptCommand(&scm_ACTOR_MALE, _char);
    return result;
}

float CleoFunctions::GET_CAR_Z_ANGLE(int car)
{
    float heading = 0;
    sautils->ScriptCommand(&scm_GET_CAR_Z_ANGLE, car, &heading);
    return heading;
}

int CleoFunctions::GET_CURRENT_WEAPON(int _char)
{
    int weaponType = 0;
    sautils->ScriptCommand(&scm_GET_CURRENT_WEAPON, _char, &weaponType);
    return weaponType;
}

void CleoFunctions::GET_WEAPON_DATA_FROM_ACTOR(int _char, int weaponSlotId, int* weaponType, int* weaponAmmo, int* weaponModel)
{
    sautils->ScriptCommand(&scm_GET_WEAPON_DATA_FROM_ACTOR, _char, weaponSlotId, weaponType, weaponAmmo, weaponModel);
}

void CleoFunctions::ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION(int _char, int object, float offsetX, float offsetY, float offsetZ, int boneId, int _p7, const char* animationName, const char* animationFile, int time)
{
    sautils->ScriptCommand(&scm_ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION, _char, object, offsetX, offsetY, offsetZ, boneId, _p7, animationName, animationFile, time);
}

void CleoFunctions::PLAY_SOUND(float x, float y, float z, int soundId)
{
    sautils->ScriptCommand(&scm_PLAY_SOUND, x, y, z, soundId);
}

void CleoFunctions::SET_OBJECT_VISIBILITY(int object, bool state)
{
    sautils->ScriptCommand(&scm_SET_OBJECT_VISIBILITY, object, state);
}

void CleoFunctions::ADD_MONEY(int player, int money)
{
    sautils->ScriptCommand(&scm_ADD_MONEY, player, money);
}

void CleoFunctions::SET_HELI_BEHAVIOR_TO_POLICE_HELI_AND_FOLLOW(int heli, int _char, int vehicle, float radius)
{
    sautils->ScriptCommand(&scm_SET_HELI_BEHAVIOR_TO_POLICE_HELI_AND_FOLLOW, heli, _char, vehicle, radius);
}

void CleoFunctions::SET_ACTOR_ANGLE_TO(int _char, float heading)
{
    sautils->ScriptCommand(&scm_SET_ACTOR_ANGLE_TO, _char, heading);
}

void CleoFunctions::PUT_ACTOR_INTO_TURRET_ON_CAR(int _char, int vehicle, float offsetX, float offsetY, float offsetZ, int position, float angleLimit, int weaponType)
{
    sautils->ScriptCommand(&scm_PUT_ACTOR_INTO_TURRET_ON_CAR, _char, vehicle, offsetX, offsetY, offsetZ, position, angleLimit, weaponType);
}

void CleoFunctions::REMOVE_ACTOR_FROM_TURRET_MODE(int _char)
{
    sautils->ScriptCommand(&scm_REMOVE_ACTOR_FROM_TURRET_MODE, _char);
}

void CleoFunctions::ENABLE_ACTOR_COLLISION_DETECTION(int _char, bool state)
{
    sautils->ScriptCommand(&scm_ENABLE_ACTOR_COLLISION_DETECTION, _char, state);
}

void CleoFunctions::SET_OBJECT_COLLISION_DETECTION(int object, bool state)
{
    sautils->ScriptCommand(&scm_SET_OBJECT_COLLISION_DETECTION, object, state);
}

void CleoFunctions::PUT_ACTOR_INTO_TURRET_ON_OBJECT(int _char, int object, float offsetX, float offsetY, float offsetZ, int orientation, float angleLimit, int weaponType)
{
    sautils->ScriptCommand(&scm_PUT_ACTOR_INTO_TURRET_ON_OBJECT, _char, object, offsetX, offsetY, offsetZ, orientation, angleLimit, weaponType);
}

void CleoFunctions::ATTACH_OBJECT_TO_ACTOR(int object, int _char, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ)
{
    sautils->ScriptCommand(&scm_ATTACH_OBJECT_TO_ACTOR, object, _char, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ);
}

int CleoFunctions::ACTOR_HEALTH(int _char)
{
    int health = 0;
    sautils->ScriptCommand(&scm_ACTOR_HEALTH, _char, &health);
    return health;
}

void CleoFunctions::SET_PLAYER_WANTED_LEVEL(int player, int wantedLevel)
{
    sautils->ScriptCommand(&scm_SET_PLAYER_WANTED_LEVEL, player, wantedLevel);
}

void CleoFunctions::DESTROY_PICKUP(int pickup)
{
    sautils->ScriptCommand(&scm_DESTROY_PICKUP, pickup);
}

int CleoFunctions::CREATE_PICKUP(int modelId, int pickupType, float x, float y, float z)
{
    int pickup = 0;
    sautils->ScriptCommand(&scm_CREATE_PICKUP, modelId, pickupType, x, y, z, &pickup);
    return pickup;
}

void CleoFunctions::SET_ACTOR_HEALTH(int _char, int health)
{
    sautils->ScriptCommand(&scm_SET_ACTOR_HEALTH, _char, health);
}

void CleoFunctions::HELI_FLY_TO(int heli, float x, float y, float z, float minAltitude, float maxAltitude)
{
    sautils->ScriptCommand(&scm_HELI_FLY_TO, heli, x, y, z, minAltitude, maxAltitude);
}

void CleoFunctions::AS_ACTOR_EXIT_CAR(int _char)
{
    sautils->ScriptCommand(&scm_AS_ACTOR_EXIT_CAR, _char);
}

float CleoFunctions::CAR_SPEED(int car)
{
    float speed = 0;
    sautils->ScriptCommand(&scm_CAR_SPEED, car, &speed);
    return speed;
}

int CleoFunctions::ACTOR_USED_CAR(int _char)
{
    int car = 0;
    sautils->ScriptCommand(&scm_ACTOR_USED_CAR, _char, &car);
    return car;
}

bool CleoFunctions::ACTOR_DRIVING(int _char)
{
    bool result = false;
    result = sautils->ScriptCommand(&scm_ACTOR_DRIVING, _char);
    return result;
}

void CleoFunctions::ACTOR_DRIVEBY(int _char, int targetChar, int targetVehicle, float x, float y, float z, float radius, int style, bool rightHandCarSeat, int fireRate)
{
    sautils->ScriptCommand(&scm_ACTOR_DRIVEBY, _char, targetChar, targetVehicle, x, y, z, radius, style, rightHandCarSeat, fireRate);
}

bool CleoFunctions::CAR_PASSENGER_SEAT_FREE(int car, int seatId)
{
    bool result = false;
    result = sautils->ScriptCommand(&scm_CAR_PASSENGER_SEAT_FREE, car, seatId);
    return result;
}

int CleoFunctions::GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(int car, int seatId)
{
    int _char = 0;
    sautils->ScriptCommand(&scm_GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT, car, seatId, &_char);
    return _char;
}

int CleoFunctions::CAR_MAX_PASSENGERS(int car)
{
    int maxPassengers = 0;
    sautils->ScriptCommand(&scm_CAR_MAX_PASSENGERS, car, &maxPassengers);
    return maxPassengers;
}

bool CleoFunctions::ACTOR_STOPPED(int _char)
{
    bool result = false;
    result = sautils->ScriptCommand(&scm_ACTOR_STOPPED, _char);
    return result;
}

bool CleoFunctions::MODEL_AVAILABLE(int modelId)
{
    bool result = false;
    result = sautils->ScriptCommand(&scm_MODEL_AVAILABLE, modelId);
    return result;
}

void CleoFunctions::LOAD_MODEL(int modelId)
{
    sautils->ScriptCommand(&scm_LOAD_MODEL, modelId);
}

void CleoFunctions::LOAD_REQUESTED_MODELS()
{
    sautils->ScriptCommand(&scm_LOAD_REQUESTED_MODELS);
}

void CleoFunctions::DESTROY_OBJECT(int object)
{
    sautils->ScriptCommand(&scm_DESTROY_OBJECT, object);
}

void CleoFunctions::DEFLATE_TIRE_ON_CAR(int car, int tireId)
{
    sautils->ScriptCommand(&scm_DEFLATE_TIRE_ON_CAR, car, tireId);
}

void CleoFunctions::SET_OBJECT_Z_ANGLE(int object, float heading)
{
    sautils->ScriptCommand(&scm_SET_OBJECT_Z_ANGLE, object, heading);
}

int CleoFunctions::CREATE_OBJECT(int modelId, float x, float y, float z)
{
    int object = 0;
    sautils->ScriptCommand(&scm_CREATE_OBJECT, modelId, x, y, z, &object);
    return object;
}

float CleoFunctions::GROUND_Z_AT(float x, float y, float z)
{
    float groundZ = 0;
    sautils->ScriptCommand(&scm_GROUND_Z_AT, x, y, z, &groundZ);
    return groundZ;
}

void CleoFunctions::PUT_CAR_AT(int car, float x, float y, float z)
{
    sautils->ScriptCommand(&scm_PUT_CAR_AT, car, x, y, z);
}

void CleoFunctions::SET_CAR_Z_ANGLE(int car, float heading)
{
    sautils->ScriptCommand(&scm_SET_CAR_Z_ANGLE, car, heading);
}

float CleoFunctions::ACTOR_Z_ANGLE(int _char)
{
    float heading = 0;
    sautils->ScriptCommand(&scm_ACTOR_Z_ANGLE, _char, &heading);
    return heading;
}

void CleoFunctions::ROTATE_AND_SHOOT(int _char, float x, float y, float z, int time)
{
    sautils->ScriptCommand(&scm_ROTATE_AND_SHOOT, _char, x, y, z, time);
}

int CleoFunctions::GET_ACTOR_MODEL(int _char)
{
    int modelId = 0;
    sautils->ScriptCommand(&scm_GET_ACTOR_MODEL, _char, &modelId);
    return modelId;
}

void CleoFunctions::AS_ACTOR_RUN_TO_ACTOR(int walking, int target, int time, float radius)
{
    sautils->ScriptCommand(&scm_AS_ACTOR_RUN_TO_ACTOR, walking, target, time, radius);
}

void CleoFunctions::SET_MAX_WANTED_LEVEL_TO(int wantedLevel)
{
    sautils->ScriptCommand(&scm_SET_MAX_WANTED_LEVEL_TO, wantedLevel);
}

void CleoFunctions::SET_PLAYER_IGNORED_BY_COPS(int player, bool state)
{
    sautils->ScriptCommand(&scm_SET_PLAYER_IGNORED_BY_COPS, player, state);
}

void CleoFunctions::PUT_TRAILER_ON_CAB(int trailer, int cab)
{
    sautils->ScriptCommand(&scm_PUT_TRAILER_ON_CAB, trailer, cab);
}

void CleoFunctions::AS_ACTOR_DRIVE_CAR_TO(int driver, int vehicle, float x, float y, float z, float speed, int driveStyle, int modelId, int drivingStyle)
{
    sautils->ScriptCommand(&scm_AS_ACTOR_DRIVE_CAR_TO, driver, vehicle, x, y, z, speed, driveStyle, modelId, drivingStyle);
}

void CleoFunctions::CLEAR_ACTOR_TASK(int _char)
{
    sautils->ScriptCommand(&scm_CLEAR_ACTOR_TASK, _char);
}

void CleoFunctions::CHANGE_PLAYER_MODEL_TO(int player, int modelId)
{
    sautils->ScriptCommand(&scm_CHANGE_PLAYER_MODEL_TO, player, modelId);
}

int CleoFunctions::GET_CAR_MODEL(int car)
{
    int modelId = 0;
    sautils->ScriptCommand(&scm_GET_CAR_MODEL, car, &modelId);
    return modelId;
}

int CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(int vehicle, int pedType, int modelId, int seatId)
{
    int ped = 0;
    sautils->ScriptCommand(&scm_CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT, vehicle, pedType, modelId, seatId, &ped);
    return ped;
}

void CleoFunctions::GIVE_ACTOR_WEAPON(int _char, int weaponType, int ammo)
{
    sautils->ScriptCommand(&scm_GIVE_ACTOR_WEAPON, _char, weaponType, ammo);
}

void CleoFunctions::SHOW_TEXT_BOX(const char* key)
{
    sautils->ScriptCommand(&scm_SHOW_TEXT_BOX, key);
}

void CleoFunctions::HELI_FOLLOW(int heli, int _char, int vehicle, float radius)
{
    sautils->ScriptCommand(&scm_HELI_FOLLOW, heli, _char, vehicle, radius);
}

void CleoFunctions::SET_CAR_ENGINE_OPERATION(int car, bool state)
{
    sautils->ScriptCommand(&scm_SET_CAR_ENGINE_OPERATION, car, state);
}

void CleoFunctions::SET_HELICOPTER_INSTANT_ROTOR_START(int heli)
{
    sautils->ScriptCommand(&scm_SET_HELICOPTER_INSTANT_ROTOR_START, heli);
}

void CleoFunctions::FADE(int time, int direction)
{
    sautils->ScriptCommand(&scm_FADE, time, direction);
}

void CleoFunctions::DESTROY_CAR(int car)
{
    sautils->ScriptCommand(&scm_DESTROY_CAR, car);
}

void CleoFunctions::SET_CAR_DOOR_STATUS(int car, int lockStatus)
{
    sautils->ScriptCommand(&scm_SET_CAR_DOOR_STATUS, car, lockStatus);
}

void CleoFunctions::ACTOR_ENTER_CAR_PASSENGER_SEAT(int _char, int vehicle, int time, int seatId)
{
    sautils->ScriptCommand(&scm_ACTOR_ENTER_CAR_PASSENGER_SEAT, _char, vehicle, time, seatId);
}

void CleoFunctions::CAR_DRIVE_TO(int car, float x, float y, float z)
{
    sautils->ScriptCommand(&scm_CAR_DRIVE_TO, car, x, y, z);
}

void CleoFunctions::FREEZE_CAR_POSITION(int car, bool state)
{
    sautils->ScriptCommand(&scm_FREEZE_CAR_POSITION, car, state);
}

void CleoFunctions::SET_MARKER_COLOR_TO(int blip, int color)
{
    sautils->ScriptCommand(&scm_SET_MARKER_COLOR_TO, blip, color);
}

void CleoFunctions::SET_ACTOR_WEAPON_AND_AMMO(int _char, int weaponType, int ammo)
{
    sautils->ScriptCommand(&scm_SET_ACTOR_WEAPON_AND_AMMO, _char, weaponType, ammo);
}

void CleoFunctions::ENABLE_CAR_SIREN(int car, bool state)
{
    sautils->ScriptCommand(&scm_ENABLE_CAR_SIREN, car, state);
}

bool CleoFunctions::ACTOR_DEAD(int actor)
{
    bool result = false;
    result = sautils->ScriptCommand(&scm_ACTOR_DEAD, actor);
    return result;
}

bool CleoFunctions::ACTOR_DEFINED(int actor)
{
    bool result = false;
    result = sautils->ScriptCommand(&scm_ACTOR_DEFINED, actor);
    return result;
}

void CleoFunctions::SET_ACTOR_WANTED_BY_POLICE(int _char, bool state)
{
    sautils->ScriptCommand(&scm_SET_ACTOR_WANTED_BY_POLICE, _char, state);
}

void CleoFunctions::KILL_ACTOR(int killer, int target)
{
    sautils->ScriptCommand(&scm_KILL_ACTOR, killer, target);
}

void CleoFunctions::FLEE_FROM_ACTOR(int _char, int threat, float radius, int time)
{
    sautils->ScriptCommand(&scm_FLEE_FROM_ACTOR, _char, threat, radius, time);
}

void CleoFunctions::STORE_PED_PATH_COORDS_CLOSEST_TO(float x, float y, float z, float* nodeX, float* nodeY, float* nodeZ)
{
    sautils->ScriptCommand(&scm_STORE_PED_PATH_COORDS_CLOSEST_TO, x, y, z, nodeX, nodeY, nodeZ);
}

int CleoFunctions::CREATE_ACTOR_PEDTYPE(int pedType, int modelId, float x, float y, float z)
{
    int _char = 0;
    sautils->ScriptCommand(&scm_CREATE_ACTOR_PEDTYPE, pedType, modelId, x, y, z, &_char);
    return _char;
}

void CleoFunctions::SET_MARKER_SIZE(int blip, int size)
{
    sautils->ScriptCommand(&scm_SET_MARKER_SIZE, blip, size);
}

bool CleoFunctions::PLAYER_AIMING_AT_ACTOR(int player, int _char)
{
    bool result = sautils->ScriptCommand(&scm_PLAYER_AIMING_AT_ACTOR, player, _char);
    return result;
}

void CleoFunctions::CAR_FOLLOR_CAR(int car, int followCar, float radius)
{
    sautils->ScriptCommand(&scm_CAR_FOLLOR_CAR, car, followCar, radius);
}

int CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(int car, int pedType, int modelId)
{
    int _char = 0;
    sautils->ScriptCommand(&scm_CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT, car, pedType, modelId, &_char);
    return _char;
}

int CleoFunctions::CREATE_CAR_AT(int modelId, float x, float y, float z)
{
    int car = 0;
    sautils->ScriptCommand(&scm_CREATE_CAR_AT, modelId, x, y, z, &car);
    return car;
}

void CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(float fromX, float fromY, float fromZ, int type, float* x, float* y, float* z)
{
    sautils->ScriptCommand(&scm_GET_NEAREST_CAR_PATH_COORDS_FROM, fromX, fromY, fromZ, type, x, y, z);
}

int CleoFunctions::CREATE_MARKER_AT(float x, float y, float z, int color, int display)
{
    int blip = 0;
    sautils->ScriptCommand(&scm_CREATE_MARKER_AT, x, y, z, color, display, &blip);
    return blip;
}

bool CleoFunctions::ACTOR_PERFORMING_ANIMATION(int _char, const char* animationName)
{
    bool result = sautils->ScriptCommand(&scm_ACTOR_PERFORMING_ANIMATION, _char, animationName);
    return result;
}

bool CleoFunctions::CAR_DEFINED(int car)
{
    bool result = sautils->ScriptCommand(&scm_CAR_DEFINED, car);
    return result;
}

void CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(int car, int drivingStyle)
{
    sautils->ScriptCommand(&scm_SET_CAR_TRAFFIC_BEHAVIOUR, car, drivingStyle);
}

void CleoFunctions::SET_CAR_MAX_SPEED(int car, float maxSpeed)
{
    sautils->ScriptCommand(&scm_SET_CAR_MAX_SPEED, car, maxSpeed);
}

void CleoFunctions::SET_CAR_TO_PSYCHO_DRIVER(int car)
{
    sautils->ScriptCommand(&scm_SET_CAR_TO_PSYCHO_DRIVER, car);
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

void CleoFunctions::REMOVE_REFERENCES_TO_CAR(int car)
{
    sautils->ScriptCommand(&scm_REMOVE_REFERENCES_TO_CAR, car);
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

void CleoFunctions::SHOW_TEXT_3NUMBERS(int gxtId, int num1, int num2, int num3, int duration, int style)
{
    char buffer[256];
    sprintf(buffer, "MPFX%i", gxtId);
    CleoFunctions::SHOW_TEXT_3NUMBERS(buffer, num1, num2, num3, duration, style);
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

int CleoFunctions::CreateMarker(float x, float y, float z, int color, int display, int size)
{
    int blip = CREATE_MARKER_AT(x, y, z, color, display);
    SET_MARKER_SIZE(blip, size);
    return blip;
}