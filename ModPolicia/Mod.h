#pragma once

#include "pch.h"

class Mod {
public:
    static const char* m_Version;
    static unsigned int m_TimePassed;
    static bool m_Enabled;
    static bool m_DevModeEnabled;
    static int m_TestWidgetId;
    static bool m_DrawTest;

    static void Update(int dt);
    static void Init();
    static void Draw();
    static void CleoInit();

    static void AddModelsToLoad();

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

    static void ToggleMod(bool enabled);
    static void ShowCredits();
};