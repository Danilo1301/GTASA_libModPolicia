#pragma once

#include <functional>

enum WAIT_FN_STATE {
    WAIT_FN_NONE,
    WAIT_FN_COMPLETED,
    WAIT_FN_CANCELLED
};

struct WaitFunction {
    int timePassed = 0;
    int time = 0;
    WAIT_FN_STATE state = WAIT_FN_STATE::WAIT_FN_NONE;
    std::function<void()> onComplete;
    std::function<void()> onCancel;
    
    bool isTestFunction = false;
    std::function<bool()> testFn;

    bool isConditionFunction = false;
    std::function<void(std::function<void()>, std::function<void()>)> conditionFn;
};

class CleoFunctions {
public:
    static std::vector<WaitFunction*> m_WaitFunctions;

    static void Update(int dt);

    static WaitFunction* AddWaitFunction(int time, std::function<void()> callback);
    static void RemoveWaitFunction(WaitFunction* waitFunction);

    static WaitFunction* AddWaitForFunction(std::function<bool()> testFn, std::function<void()> callback);
    static WaitFunction* AddCondition(std::function<void(std::function<void()>, std::function<void()>)> fn, std::function<void()> onComplete, std::function<void()> onCancel);

    static void WAIT(int time, std::function<void()> callback);

    static void SET_ACTOR_ARMED_WEAPON(int _char, int weaponType);
    static void AIM_AT_ACTOR(int _char, int target, int time);
    static bool ACTOR_MALE(int _char);
    static float GET_CAR_Z_ANGLE(int car);
    static int GET_CURRENT_WEAPON(int _char);
    static void GET_WEAPON_DATA_FROM_ACTOR(int _char, int weaponSlotId, int* weaponType, int* weaponAmmo, int* weaponModel);
    static void ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION(int _char, int object, float offsetX, float offsetY, float offsetZ, int boneId, int _p7, const char* animationName, const char* animationFile, int time);
    static void PLAY_SOUND(float x, float y, float z, int soundId);
    static void SET_OBJECT_VISIBILITY(int object, bool state);
    static void ADD_MONEY(int player, int money);
    static void SET_HELI_BEHAVIOR_TO_POLICE_HELI_AND_FOLLOW(int heli, int _char, int vehicle, float radius);
    static void SET_ACTOR_ANGLE_TO(int _char, float heading);
    static void PUT_ACTOR_INTO_TURRET_ON_CAR(int _char, int vehicle, float offsetX, float offsetY, float offsetZ, int position, float angleLimit, int weaponType);
    static void REMOVE_ACTOR_FROM_TURRET_MODE(int _char);
    static void ENABLE_ACTOR_COLLISION_DETECTION(int _char, bool state);
    static void SET_OBJECT_COLLISION_DETECTION(int object, bool state);
    static void PUT_ACTOR_INTO_TURRET_ON_OBJECT(int _char, int object, float offsetX, float offsetY, float offsetZ, int orientation, float angleLimit, int weaponType);
    static void ATTACH_OBJECT_TO_ACTOR(int object, int _char, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ);
    static int ACTOR_HEALTH(int _char);
    static void SET_PLAYER_WANTED_LEVEL(int player, int wantedLevel);
    static void DESTROY_PICKUP(int pickup);
    static int CREATE_PICKUP(int modelId, int pickupType, float x, float y, float z);
    static void SET_ACTOR_HEALTH(int _char, int health);
    static void HELI_FLY_TO(int heli, float x, float y, float z, float minAltitude, float maxAltitude);
    static void AS_ACTOR_EXIT_CAR(int _char);
    static float CAR_SPEED(int car);
    static int ACTOR_USED_CAR(int _char);
    static bool ACTOR_DRIVING(int _char);
    static void ACTOR_DRIVEBY(int _char, int targetChar, int targetVehicle, float x, float y, float z, float radius, int style, bool rightHandCarSeat, int fireRate);
    static bool CAR_PASSENGER_SEAT_FREE(int car, int seatId);
    static int GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(int car, int seatId);
    static int CAR_MAX_PASSENGERS(int car);
    static bool ACTOR_STOPPED(int _char);
    static bool MODEL_AVAILABLE(int modelId);
    static void LOAD_MODEL(int modelId);
    static void LOAD_REQUESTED_MODELS();
    static void DESTROY_OBJECT(int object);
    static void DEFLATE_TIRE_ON_CAR(int car, int tireId);
    static void SET_OBJECT_Z_ANGLE(int object, float heading);
    static int CREATE_OBJECT(int modelId, float x, float y, float z);
    static float GROUND_Z_AT(float x, float y, float z);
    static void PUT_CAR_AT(int car, float x, float y, float z);
    static void SET_CAR_Z_ANGLE(int car, float heading);
    static float ACTOR_Z_ANGLE(int _char);
    static void ROTATE_AND_SHOOT(int _char, float x, float y, float z, int time);
    static int GET_ACTOR_MODEL(int _char);
    static void AS_ACTOR_RUN_TO_ACTOR(int walking, int target, int time, float radius);
    static void SET_MAX_WANTED_LEVEL_TO(int wantedLevel);
    static void SET_PLAYER_IGNORED_BY_COPS(int player, bool state);
    static void PUT_TRAILER_ON_CAB(int trailer, int cab);
    static void AS_ACTOR_DRIVE_CAR_TO(int driver, int vehicle, float x, float y, float z, float speed, int driveStyle, int modelId, int drivingStyle);
    static void CLEAR_ACTOR_TASK(int _char);
    static void CHANGE_PLAYER_MODEL_TO(int player, int modelId);
    static int GET_CAR_MODEL(int car);
    static int CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(int vehicle, int pedType, int modelId, int seatId);
    static void GIVE_ACTOR_WEAPON(int _char, int weaponType, int ammo);
    static void SHOW_TEXT_BOX(const char* key);
    static void HELI_FOLLOW(int heli, int _char, int vehicle, float radius);
    static void SET_CAR_ENGINE_OPERATION(int car, bool state);
    static void SET_HELICOPTER_INSTANT_ROTOR_START(int heli);
    static void FADE(int time, int direction);
    static void DESTROY_CAR(int car);
    static void SET_CAR_DOOR_STATUS(int car, int lockStatus);
    static void ACTOR_ENTER_CAR_PASSENGER_SEAT(int _char, int vehicle, int time, int seatId);
    static void CAR_DRIVE_TO(int car, float x, float y, float z);
    static void FREEZE_CAR_POSITION(int car, bool state);
    static void SET_MARKER_COLOR_TO(int blip, int color);

    //doesnt work for some reason
    static void SET_ACTOR_WEAPON_AND_AMMO(int _char, int weaponType, int ammo);
    static void ENABLE_CAR_SIREN(int car, bool state);
    static bool ACTOR_DEAD(int actor);
    static bool ACTOR_DEFINED(int actor);
    static void SET_ACTOR_WANTED_BY_POLICE(int _char, bool state);
    static void KILL_ACTOR(int killer, int target);
    static void FLEE_FROM_ACTOR(int _char, int threat, float radius, int time);
    static void STORE_PED_PATH_COORDS_CLOSEST_TO(float x, float y, float z, float* nodeX, float* nodeY, float* nodeZ);
    static int CREATE_ACTOR_PEDTYPE(int pedType, int modelId, float x, float y, float z);
    static void SET_MARKER_SIZE(int blip, int size);
    static bool PLAYER_AIMING_AT_ACTOR(int player, int _char);
    static void CAR_FOLLOR_CAR(int car, int followCar, float radius);
    static int CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(int car, int pedType, int modelId);
    static int CREATE_CAR_AT(int modelId, float x, float y, float z);
    static void GET_NEAREST_CAR_PATH_COORDS_FROM(float fromX, float fromY, float fromZ, int type, float* x, float* y, float* z);
    static int CREATE_MARKER_AT(float x, float y, float z, int color, int display);
    static bool ACTOR_PERFORMING_ANIMATION(int _char, const char* animationName);
    static bool CAR_DEFINED(int car);
    static void SET_CAR_TRAFFIC_BEHAVIOUR(int car, int drivingStyle);
    static void SET_CAR_MAX_SPEED(int car, float maxSpeed);
    static void SET_CAR_TO_PSYCHO_DRIVER(int car);
    static void DESTROY_ACTOR(int actor);
    static void DESTROY_SPHERE(int sphere);
    static int CREATE_SPHERE(float x, float y, float z, float radius);
    static void PUT_ACTOR_IN_GROUP(int group, int _char);
    static void PUT_ACTOR_IN_GROUP_AS_LEADER(int group, int _char);
    static int GET_PLAYER_GROUP(int player);
    static void ENTER_CAR_AS_DRIVER_AS_ACTOR(int _char, int vehicle, int time);
    static void EXIT_CAR_AS_ACTOR(int _actor);
    static void CAR_TURN_OFF_ENGINE(int car);
    static int GET_DRIVER_OF_CAR(int car);
    static void STORE_COORDS_FROM_CAR_WITH_OFFSET(int car, float offsetX, float offsetY, float offsetZ, float* x, float* y, float* z);
    static void STORE_COORDS_FROM_ACTOR_WITH_OFFSET(int _char, float offsetX, float offsetY, float offsetZ, float* x, float* y, float* z);
    static int GET_CAR_IN_SPHERE(float x, float y, float z, float radius, int modelVehicle);
    static bool IS_CHAR_IN_ANY_CAR(int _char);
    static void REMOVE_REFERENCES_TO_CAR(int car);
    static void REMOVE_REFERENCES_TO_ACTOR(int _char);
    static void SET_PLAYER_CAN_MOVE(int player, bool state);
    static void PERFORM_ANIMATION_AS_ACTOR(int _char, const char* animationName, const char* animationFile, float frameDelta, bool loop, bool lockX, bool lockY, bool lockF, int time);
    static int GET_PLAYER_ACTOR(int player);
    static void GET_CHAR_COORDINATES(int _char, float* x, float* y, float* z);
    static void SHOW_TEXT_2NUMBERS_STYLED(const char* key, int num1, int num2, int duration, int style);
    static void SHOW_TEXT_3NUMBERS(const char* key, int num1, int num2, int num3, int duration, int style);
    static void SHOW_TEXT_3NUMBERS(int gxtId, int num1, int num2, int num3, int duration, int style);

    //issue: its not detecting gang members and police
    static int GET_RANDOM_CHAR_IN_SPHERE(float x, float y, float z, float radius, bool civilian, bool gang, bool criminal);
    static void DISABLE_MARKER(int blip);
    static int ADD_BLIP_FOR_CAR(int car);
    static int ADD_BLIP_FOR_CHAR(int _char);
    static bool IS_WIDGET_PRESSED(int widgetId);
    static bool PLAYER_DEFINED(int player);
    static bool HAS_ANIMATION_LOADED(const char* animationFile);
    static void LOAD_ANIMATION(const char* animationFile);

    static int CreateMarker(float x, float y, float z, int color, int display, int size);
};