#include "Pullover.h"

#include "Widgets.h"
#include "Log.h"
#include "Peds.h"
#include "Vehicles.h"
#include "CleoFunctions.h"

#include "windows/WindowTest.h"
#include "windows/WindowFrisk.h"
#include "windows/WindowPullover.h"

double Pullover::PULLOVER_MIN_DISTANCE_PED = 1;
double Pullover::PULLOVER_MIN_DISTANCE_VEHICLE = 2;
double Pullover::PULLOVER_MAX_DISTANCE = 30;

Ped* Pullover::m_PullingPed = NULL;
Vehicle* Pullover::m_PullingVehicle = NULL;
Ped* Pullover::m_ScorchingPed = NULL;
int Pullover::m_ScorchingPedSphere = 0;

PULLOVER_TYPE Pullover::m_FriskType = PULLOVER_TYPE::PULLING_NONE;

void Pullover::Update(int dt)
{
    if(!m_PullingPed && !m_PullingVehicle)
    {
        if(Widgets::IsWidgetJustPressed(7))
        {
            Log::file << "[Mod] Widget 7 has just been pressed" << std::endl;

            TryPullOverPedOrCar();
        }
    }

    if(m_ScorchingPed)
    {
        CVector dpPosition = CVector(1536.1325, -1671.2093, 13.3828);

        float x = 0, y = 0, z = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(m_ScorchingPed->hPed, 0, 0, 0, &x, &y, &z);

        auto distance = DistanceBetweenPoints(dpPosition, CVector(x, y, z));

        if(distance < 3.0f)
        {
            /*
            01C2: remove_references_to_actor 4@ 
            009B: destroy_actor 4@ 
            03BD: destroy_sphere 7@
            */

            CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(m_ScorchingPed->hPed);

            CleoFunctions::DESTROY_ACTOR(m_ScorchingPed->hPed);
            m_ScorchingPed = NULL;
            m_PullingPed = NULL;
            m_PullingVehicle = NULL;

            CleoFunctions::DESTROY_SPHERE(m_ScorchingPedSphere);
            m_ScorchingPedSphere = 0;
        }
    }
}

void Pullover::TryPullOverPedOrCar()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0.0f, y = 0.0f, z = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0.0f, 4.0f, 0.0f, &x, &y, &z);

    int randomCar = CleoFunctions::GET_CAR_IN_SPHERE(x, y, z, 5.0f, -1);

    if(randomCar > 0)
    {
        int driver = CleoFunctions::GET_DRIVER_OF_CAR(randomCar);
        bool hasNoDriver = driver <= 0;
        Log::file << "driver: " << driver << std::endl;

        if(hasNoDriver)
        {
            TryPullOverPed();
            return;
        }
    }

    if(randomCar > 0)
    {
        if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor))
                CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "CopTraf_Stop", "POLICE", 4.0f, 0, 0, 0, 0, -1);

        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX31", 0, 0, 0, 2000, 1); //parado!

        /*
        float carX = 0.0f, carY = 0.0f, carZ = 0.0f;
        CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(randomCar, 0.0f, 0.0f, 0.0f, &carX, &carY, &carZ);
        Log::file << "carX: " << carX << std::endl;
        Log::file << "carY: " << carY << std::endl;
        Log::file << "carZ: " << carZ << std::endl;
        */

        m_PullingVehicle = Vehicles::TryCreateVehicle(randomCar);
        m_PullingVehicle->UpdateInventory();
        m_PullingVehicle->blip = CleoFunctions::ADD_BLIP_FOR_CAR(randomCar);

        int driver = CleoFunctions::GET_DRIVER_OF_CAR(randomCar);
        m_PullingPed = Peds::TryCreatePed(driver);
        m_PullingPed->UpdateInventory();
        m_PullingPed->vehicleOwned = m_PullingVehicle;
        m_PullingPed->blip = CleoFunctions::ADD_BLIP_FOR_CHAR(driver);

        CleoFunctions::CAR_TURN_OFF_ENGINE(randomCar);
        
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX63", 0, 0, 0, 3000, 1); //chegue mais perto

        //wait to get closer to the car
        CleoFunctions::AddWaitForFunction([playerActor, randomCar] () {
            
            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) return false;

            auto distance = GetDistanceBetweenPedAndCar(playerActor, randomCar);

            Log::file << "distance from car: " << distance << std::endl;

            if(distance < PULLOVER_MIN_DISTANCE_VEHICLE) return true;
            if(distance > PULLOVER_MAX_DISTANCE) return true;

            return false;
        },
        [playerActor, randomCar] () {
            auto distance = GetDistanceBetweenPedAndCar(playerActor, randomCar);

            if(distance <= PULLOVER_MIN_DISTANCE_VEHICLE)
            {
                Log::file << "Create pulling car menu" << std::endl;
                WindowPullover::CreatePullingCar();
            } else {
                Log::file << "Car is too far away!" << std::endl;
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX56", 0, 0, 0, 3000, 1); //muito longe
            }
        });
    } else {
        TryPullOverPed();
    }
}

void Pullover::TryPullOverPed()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0.0f, y = 0.0f, z = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0.0f, 4.0f, 0.0f, &x, &y, &z);

    int randomChar = CleoFunctions::GET_RANDOM_CHAR_IN_SPHERE(x, y, z, 4.0f, true, true, true);

    if(randomChar == -1)
    {
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX32", 0, 0, 0, 2000, 1); //nao encontrado
        return;
    }

    if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor))
        CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "CopTraf_Stop", "POLICE", 4.0f, 0, 0, 0, 0, -1);

    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX31", 0, 0, 0, 2000, 1); //parado!

    m_PullingPed = Peds::TryCreatePed(randomChar);
    m_PullingPed->UpdateInventory();

    m_PullingPed->blip = CleoFunctions::ADD_BLIP_FOR_CHAR(randomChar);

    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(randomChar, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1);

    CleoFunctions::WAIT(2000, [playerActor, randomChar]() {
        WindowPullover::CreatePullingPed();
    });
}

void Pullover::FriskPed()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "hndshkfa_swt", "gangs", 2.0f, 0, 0, 0, 0, -1);

    m_PullingPed->UpdateInventory();

    CleoFunctions::WAIT(3500, []() {
        WindowFrisk::Create();
    });
}

void Pullover::FriskVehicle()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(playerActor, "hndshkfa_swt", "gangs", 2.0f, 0, 0, 0, 0, -1);

    CleoFunctions::WAIT(3500, []() {
        WindowFrisk::CreateFriskCar();
    });
}

void Pullover::FreePed()
{
    //CleoFunctions::SET_PLAYER_CAN_MOVE(m_PullingPed->hPed, true);
    CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(m_PullingPed->hPed);

    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX55", 0, 0, 0, 3000, 1); //liberado

    if(m_PullingPed->blip > 0)
    {
        CleoFunctions::DISABLE_MARKER(m_PullingPed->blip);
        m_PullingPed->blip = 0;
    }

    m_PullingPed = NULL;
}

void Pullover::FreeVehicle()
{
    CleoFunctions::ENTER_CAR_AS_DRIVER_AS_ACTOR(m_PullingPed->hPed, m_PullingPed->vehicleOwned->hVehicle, 20000);

    //TODO: make it vehicle->RemoveBlip();
    if(m_PullingVehicle->blip > 0)
    {
        CleoFunctions::DISABLE_MARKER(m_PullingVehicle->blip);
        m_PullingVehicle->blip = 0;
    }

    if(m_PullingPed->blip > 0)
    {
        CleoFunctions::DISABLE_MARKER(m_PullingPed->blip);
        m_PullingPed->blip = 0;
    }

    m_PullingPed = NULL;
    m_PullingVehicle = NULL;
}

double Pullover::GetDistanceBetweenPedAndCar(int hChar, int hVehicle)
{
    float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
    CleoFunctions::GET_CHAR_COORDINATES(hChar, &playerX, &playerY, &playerZ);

    float carX = 0.0f, carY = 0.0f, carZ = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(hVehicle, 0.0f, 0.0f, 0.0f, &carX, &carY, &carZ);

    auto distance = DistanceBetweenPoints(CVector(playerX, playerY, playerZ), CVector(carX, carY, carZ));
    return distance;
}

double Pullover::GetDistanceBetweenPeds(int hChar, int hChar2)
{
    float x1 = 0.0f, y1 = 0.0f, z1 = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(hChar, 0, 0, 0, &x1, &y1, &z1);

    float x2 = 0.0f, y2 = 0.0f, z2 = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(hChar2, 0, 0, 0, &x2, &y2, &z2);

    auto distance = DistanceBetweenPoints(CVector(x1, y1, z1), CVector(x2, y2, z2));
    return distance;
}