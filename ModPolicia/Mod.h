#pragma once

#include "pch.h"

class Mod {
public:
    static const char* m_Version;
    static unsigned int m_TimePassed;
    static bool m_Enabled;

    static void Update(int dt);
    static void Load();
    
    static int GetRandomNumber(int min, int max);

    /**
      *   \brief Calculate probability
      *
      *   Description
      *   
      *   \param chance Float from 0.0 to 1.0
      *   \return Bool
      **/
    static bool CalculateProbability(float chance);

    static void ProcessMenuButtons(int dt);
    static void ProcessTestMenuButtons(int dt);

    static CVector GetCarPosition(int hVehicle);
    static CVector GetPedPosition(int hPed);
    static bool IsActorAliveAndDefined(int hPed);

    static void ToggleMod(bool enabled);
};