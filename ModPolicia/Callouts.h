#pragma once

#include "pch.h"

#include "Ped.h"

enum CALLOUT_TYPE {
    CALLOUT_ASSAULT
};

struct Callout
{
    CALLOUT_TYPE type;
    int gxtId;
};

class Callouts {
public:
    static int m_TimeBetweenCallouts;
    static int m_TimeToCallout;
    static std::vector<Callout> m_Callouts;
    static int m_ModulatingCalloutIndex;
    static int m_CurrentCalloutIndex;
    static std::vector<Ped*> m_Criminals;
    static bool m_AproachingCallout;

    static void Update(int dt);
    
    static void UpdateCriminals(int dt);

    static bool IsModulatingCallout();
    static Callout GetCurrentCallout();

    static void StartAssaultCallout();
};