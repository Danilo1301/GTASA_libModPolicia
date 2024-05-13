#pragma once

#include <functional>

struct WaitFunction {
    int timePassed = 0;
    int time = 0;
    std::function<void()> callback;
    
    bool isTestFunction = false;
    std::function<bool()> testFn;
};

class CleoFunctions {
public:
    static std::vector<WaitFunction*> m_WaitFunctions;

    static void Update(int dt);

    static WaitFunction* AddWaitFunction(int time, std::function<void()> callback);
    static WaitFunction* AddWaitForFunction(std::function<bool()> testFn, std::function<void()> callback);
    static void RemoveWaitFunction(WaitFunction* waitFunction);

    static void WAIT(int time, std::function<void()> callback);

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
    static void REMOVE_REFERENCES_TO_ACTOR(int _char);
    static void SET_PLAYER_CAN_MOVE(int player, bool state);
    static void PERFORM_ANIMATION_AS_ACTOR(int _char, const char* animationName, const char* animationFile, float frameDelta, bool loop, bool lockX, bool lockY, bool lockF, int time);
    static int GET_PLAYER_ACTOR(int player);
    static void GET_CHAR_COORDINATES(int _char, float* x, float* y, float* z);
    static void SHOW_TEXT_2NUMBERS_STYLED(const char* key, int num1, int num2, int duration, int style);
    static void SHOW_TEXT_3NUMBERS(const char* key, int num1, int num2, int num3, int duration, int style);

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