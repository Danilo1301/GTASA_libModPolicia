#pragma once

class Mod {
public:
    static const char* m_Version;
    static unsigned int m_TimePassed;

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
};