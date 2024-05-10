#pragma once

#include <functional>

struct WaitFunction {
    int timePassed = 0;
    int time = 0;
    std::function<void()> callback;
};

class CleoFunctions {
public:
    static std::vector<WaitFunction*> m_WaitFunctions;

    static void Update(int dt);

    static WaitFunction* AddWaitFunction(int time, std::function<void()> callback);
    static void RemoveWaitFunction(WaitFunction* waitFunction);

    static void WAIT(int time, std::function<void()> callback);

    static void REMOVE_REFERENCES_TO_ACTOR(int _char);
    static void SET_PLAYER_CAN_MOVE(int player, bool state);
    static void PERFORM_ANIMATION_AS_ACTOR(int _char, const char* animationName, const char* animationFile, float frameDelta, bool loop, bool lockX, bool lockY, bool lockF, int time);
    static int GET_PLAYER_ACTOR(int player);
    static void GET_CHAR_COORDINATES(int _char, float* x, float* y, float* z);
    static void SHOW_TEXT_2NUMBERS_STYLED(const char* key, int num1, int num2, int duration, int style);
    static void SHOW_TEXT_3NUMBERS(const char* key, int num1, int num2, int num3, int duration, int style);

    //issue: its not detecting gang members and police
    static int GET_RANDOM_CHAR_IN_SPHERE(float x, float y, float z, float radius, bool civilian, bool gang, bool criminal);
    static int ADD_BLIP_FOR_CHAR(int _char);
    static bool IS_WIDGET_PRESSED(int widgetId);
    static bool PLAYER_DEFINED(int player);
    static bool HAS_ANIMATION_LOADED(const char* animationFile);
    static void LOAD_ANIMATION(const char* animationFile);
};