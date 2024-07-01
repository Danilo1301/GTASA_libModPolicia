#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "Inventory.h"

#include "TrunkData.h"

enum ACTION_STATUS {
    ACTION_NONE = 0,

    SCORCH_GOING_TO_PED,
    SCORCH_WAITING_FOR_PED_TO_ENTER,

    GOING_TO_SUSPECT,

    WAITING_FOR_PEDS_TO_ENTER_CAR_AND_LEAVE,
    LEAVING_SCENE,

    WAITING_FOR_PEDS_TO_EXIT,

    WAITING_FOR_PEDS_TO_ENTER_CAR_AND_DO_NOTHING,

    //CHASE_CHASING,

    //CALLOUT_GOING_TO_CALLOUT,
    //CALLOUT_ACTING_ON_CALLOUT,

    TOWING_GOING_TO_CAR,

    MEDIC_GOING_TO_LOCATION
};

class Vehicle {
public:
    static float CHANCE_VEHICLE_BEEING_STOLEN;
    static float CHANCE_VEHICLE_HAVING_GUNS;

    int hVehicle;
    CVehicle* pVehicle = NULL;
    int modelId;

    int blip = 0;
    
    bool isStolen = false;

    Inventory* inventory = new Inventory();

    //int hDriver = 0;
    //std::vector<int> hPassengers;

    int hDriverOwner = 0;
    std::vector<int> hPassengersOwner;

    CVector fromPos = CVector(0, 0, 0);
    CVector drivingTo = CVector(0, 0, 0);
    int goingToPed = 0;
    int followingCar = 0;

    ACTION_STATUS actionStatus = ACTION_STATUS::ACTION_NONE;

    int towingVehicle = 0;

    float prevSpeed = 0.0f;
    bool hasJustCrashed = false;

    TrunkData* trunk = NULL;

    bool freezeCarPosition = false;

    Vehicle(int hVehicle);
	~Vehicle();

    void Update(int dt);
    void UpdateLeaveScene();
    void UpdateCarMenuWidget();

    void UpdateInventory();

    bool IsPoliceCar();
    bool IsPoliceHelicopter();
    
    bool HasIlegalStuff();
    bool HasGuns();

    int AddBlip();
    int AddBlip(int color);
    void RemoveBlip();

    void MakePedsEnterVehicleAndLeaveScene();
    void MakePedsEnterVehicle();
    void MakePedsExitCar();

    void CheckDriverAndPassengersAreAlive();
    void ReplaceDriverByAnyPassenger();

    void RemoveDriverAndPassengersBlip();

    std::vector<int> GetPassengers();
    int GetDriver();
    std::vector<int> GetDriverAndPassengers();
    std::vector<int> GetOwners();

    void SetDriverAndPassengersOwners();

    bool IsAllDriverAndPassengersInsideCar();
    bool IsAllDriverAndPassengersOutsideCar();

    float GetSpeed();
};