#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"

enum FRISK_TYPE {
    FRISK_NONE,
    FRISK_PED,
    FRISK_VEHICLE
};

class Pullover {
public:
    static double PULLOVER_MIN_DISTANCE_PED;
    static double PULLOVER_MIN_DISTANCE_VEHICLE;
    static double PULLOVER_MAX_DISTANCE;

    static Ped* m_PullingPed;
    static Vehicle* m_PullingVehicle;

    static FRISK_TYPE m_FriskType;

    static void Update(int dt);
    static void UpdateWidgetPress(int dt);
    static void UpdatePullingPed(int dt);

    static void PullOverPed(int hPed);
    static void TryPullOverCar();
    static void PullOverCar(int hVehicle);
    
    static int FindAimingPed();

    static void CreateTestPullOverPed();
    static void CreateTestPullOverVehicle();

    static void FriskPed();
    static void FriskVehicle();

    static void FreePed();
    static void MakePedWait();
    static void FreeVehicle();

    static void AskPedsToLeaveCar(Vehicle* vehicle);
    static void AskPedToStopCarOnRight(Vehicle* vehicle);

    static double GetDistanceBetweenPedAndCar(int hChar, int hVehicle);
    static double GetDistanceBetweenPeds(int hChar, int hChar2);
};