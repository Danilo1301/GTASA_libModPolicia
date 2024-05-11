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


    static PULLOVER_TYPE m_FriskType;

    static void Update(int dt);
    static void TryPullOverPedOrCar();
    static void TryPullOverPed();
    static void FriskPed();
    static void FriskVehicle();
    static void FreePed();
    static void FreeVehicle();
    static double GetDistanceBetweenPedAndCar(int hChar, int hVehicle);
    static double GetDistanceBetweenPeds(int hChar, int hChar2);
};