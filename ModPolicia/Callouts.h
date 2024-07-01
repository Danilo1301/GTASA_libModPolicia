#pragma once

#include "pch.h"

#include "Ped.h"
#include "audiosystem.h"

enum CALLOUT_TYPE {
    CALLOUT_NONE = -1,
    CALLOUT_ASSAULT,
    CALLOUT_GANG_SHOTS_FIRED,
    CALLOUT_STOLEN_VEHICLE,
    CALLOUT_HOUSE_INVASION,
    CALLOUT_CHASE
};

/*
CALLOUT_TYPE type;
int gxtId;
float chance;
std::string audio;
*/
struct Callout
{
    CALLOUT_TYPE type;
    int gxtId;
    float chance;
    std::string audio;
};

class Callouts {
public:
    static float CALLOUT_DISTANCE;
    static int m_TimeBetweenCallouts;
    static int m_TimeToCallout;
    static std::vector<Callout> m_Callouts;
    static CALLOUT_TYPE m_ModulatingCalloutIndex;
    static CALLOUT_TYPE m_CurrentCalloutIndex;
private:
    static std::vector<Ped*> m_Criminals;
public:
    static bool m_AproachingCallout;
    static bool m_AbortedCallout;
    static bool m_WaitingToPlayAcceptCalloutAudio;
    static CAudioStream* m_ModulatingCalloutAudio;

    static std::vector<int> m_StolenVehicleIds;
    static std::vector<int> m_StolenTruckIds;

    static void Update(int dt);
    
    static void UpdateCriminals(int dt);

    static CALLOUT_TYPE GetRandomCallout();

    static void AddPedToCriminalList(Ped* ped);
    static void RemovePedFromCriminalList(Ped* ped);
    static bool IsPedOnCriminalList(Ped* ped);
    static std::vector<Ped*> GetCriminals();

    static void AbortCallout();

    static bool IsModulatingCallout();

    static bool IsOnCallout();

    static Callout GetCurrentCallout();

    static void StartAssaultCallout();
    static void StartGangShotsFiredCallout();
    static void StartStolenVehicleCallout();
    static void StartHouseInvasionCallout();
    static void StartChaseCallout();

    static void AproachCallout(CVector location, float aproachDistance, std::function<void(CVector)> onReachMarker, std::function<void()> onAbort);
    static void AproachCalloutPedPath(float aproachDistance, std::function<void(CVector)> onReachMarker);
    static void AproachCalloutCarPath(float aproachDistance, std::function<void(CVector)> onReachMarker);

    static Ped* SpawnPedInRandomPedPathLocation(int pedType, int modelId, CVector position, float radius);

    static Ped* GetClosestCriminal(CVector fromPosition);
};