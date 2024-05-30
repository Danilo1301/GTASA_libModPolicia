#pragma once

#include "pch.h"

#include "Ped.h"

enum CALLOUT_TYPE {
    CALLOUT_NONE = -1,
    CALLOUT_ASSAULT,
    GANG_SHOTS_FIRED,
    STOLEN_VEHICLE,
    HOUSE_INVASION
};

struct Callout
{
    CALLOUT_TYPE type;
    int gxtId;
    float chance;
};

enum SkinGenre {
    SKIN_MALE,
    SKIN_FEMALE,
    BOTH
};

enum SkinGang {
    GANG_NONE,
    GANG_BALLAS,
    GANG_VAGOS,
    GANG_AZTECAS
};

/*
int modelId;
SkinGenre genre;
SkinGang gang;
*/
struct SkinData {
    int modelId;
    SkinGenre genre;
    SkinGang gang;
};

class Callouts {
public:
    static float CALLOUT_DISTANCE;
    static int m_TimeBetweenCallouts;
    static int m_TimeToCallout;
    static std::vector<Callout> m_Callouts;
    static CALLOUT_TYPE m_ModulatingCalloutIndex;
    static CALLOUT_TYPE m_CurrentCalloutIndex;
    static std::vector<Ped*> m_Criminals;
    static bool m_AproachingCallout;

    static std::vector<SkinData> m_Skins;

    static void Update(int dt);
    
    static void UpdateCriminals(int dt);

    static CALLOUT_TYPE GetRandomCallout();

    static bool IsModulatingCallout();

    static bool IsOnCallout();

    static Callout GetCurrentCallout();

    static void StartAssaultCallout();
    static void StartGangShotsFiredCallout();
    static void StartStolenVehicleCallout();
    static void StartHouseInvasionCallout();

    static void AproachCallout(CVector location, float aproachDistance, std::function<void(CVector)> onReachMarker);
    static void AproachCalloutPedPath(std::function<void(CVector)> onReachMarker);

    static Ped* SpawnPedInRandomPedPathLocation(int pedType, int modelId, CVector position, float radius);

    static SkinData GetRandomSkin(SkinGenre genre, SkinGang gang);

    static Ped* GetClosestCriminal(CVector fromPosition);
};