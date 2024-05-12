#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"

//change to FRISK_TYPE
enum PULLOVER_TYPE {
    PULLING_NONE,
    PULLING_PED,
    PULLING_VEHICLE
};

class Pullover {
public:
    static double PULLOVER_MIN_DISTANCE_PED;
    static double PULLOVER_MIN_DISTANCE_VEHICLE;
    static double PULLOVER_MAX_DISTANCE;

    static Ped* m_PullingPed;
    static Vehicle* m_PullingVehicle;
    static Ped* m_ScorchingPed;
    static int m_ScorchingPedSphere;
    static int m_ScorchingPedBlip;
    static int m_ScorchingToDpIndex;

    static std::vector<CVector> m_PoliceDepartmentPositions;


    static PULLOVER_TYPE m_FriskType;

    static void Update(int dt);
    static void PullOverPed(int hPed);
    static void TryPullOverCar();
    static int FindAimingPed();
    static void FriskPed();
    static void FriskVehicle();
    static void FreePed();
    static void MakePedWait();
    static void FreeVehicle();
    static void AskPedToLeaveCar(Ped* ped);
    static void SartScorchingPed(Ped* ped);
    static double GetDistanceBetweenPedAndCar(int hChar, int hVehicle);
    static double GetDistanceBetweenPeds(int hChar, int hChar2);
    static int GetClosestPoliceDepartment();
};