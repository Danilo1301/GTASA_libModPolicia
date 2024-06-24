#include "Stats.h"

#include "ModConfig.h"
#include "Log.h"

int Stats::TimesOpenedGame = 0;
int Stats::TimePlayed = 0;
int Stats::m_msTimePlayed = 0;

void Stats::Update(int dt)
{
    m_msTimePlayed += dt;

    if(m_msTimePlayed > 5000)
    {
        int add = (int)((float)m_msTimePlayed / 1000.0f);
        TimePlayed += add;

        Log::Level(LOG_LEVEL::LOG_BOTH) << "m_msTimePlayed was " << m_msTimePlayed << " dt: " << dt << ", add: " << add << std::endl;
        m_msTimePlayed -= 5000;

        ModConfig::SaveStats();
    }
}