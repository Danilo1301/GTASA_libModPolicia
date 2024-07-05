#include "pch.h"

#include "Vehicle.h"
#include "Ped.h"

class EmergencyVehicleSystem {
public:
    std::vector<Vehicle*> m_Vehicles;
    std::vector<Ped*> m_Peds;

    EmergencyVehicleSystem(int vehicleModelId, int pedModelId, bool ressurectPed);

    int m_VehicleModelId = 416;
    int m_PedModelId = 274;
    bool m_RessurectPed = true;

    void Update(int dt);

    void CallVehicle(CVector location);
    
    static void TakeBody(Ped* medic, bool ressurect);

    std::vector<Ped*> GetAllMedics();
};

class Ambulance {
public:
    static EmergencyVehicleSystem* m_AmbulanceSystem;
    static EmergencyVehicleSystem* m_IMLSystem;
    static float SPAWN_EMERGENCY_VEHICLES_DISTANCE;

    static void Init();

    static void Update(int dt);

    static void CallAmbulance(CVector location);
    static void CallIML(CVector location);

    static Ped* GetDeadPedThatIsFree(Ped* medic);
    static std::vector<Ped*> GetAllMedics();
    static bool IsAnyMedicGoingToPed(Ped* ped);
};