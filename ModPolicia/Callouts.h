#pragma once

#include "pch.h"

#include "Ped.h"

enum CALLOUT_TYPE {
    CALLOUT_ASSAULT,
    GANG_SHOTS_FIRED
};

struct Callout
{
    CALLOUT_TYPE type;
    int gxtId;
    float chance;
};

class Callouts {
public:
    static float CALLOUT_DISTANCE;
    static int m_TimeBetweenCallouts;
    static int m_TimeToCallout;
    static std::vector<Callout> m_Callouts;
    static int m_ModulatingCalloutIndex;
    static int m_CurrentCalloutIndex;
    static std::vector<Ped*> m_Criminals;
    static bool m_AproachingCallout;

    static void Update(int dt);
    
    static void UpdateCriminals(int dt);

    static int GetRandomCallout();

    static bool IsModulatingCallout();

    static bool IsOnCallout();

    static Callout GetCurrentCallout();

    static void StartAssaultCallout();
    static void StartGangShotsFiredCallout();

    static Ped* SpawnPedInRandomPedPathLocation(int pedType, int modelId, CVector position, float radius);
};