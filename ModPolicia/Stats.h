#pragma once

#include "pch.h"

class Stats {
public:
    static int TimesOpenedGame;
    static int TimePlayed;
    static int m_msTimePlayed;

    static void Update(int dt);
};