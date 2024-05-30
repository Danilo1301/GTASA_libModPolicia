#pragma once

#include "pch.h"

#include "SimpleGTA.h"

class Locations {
public:
    static std::vector<CVector> m_Houses;
    static std::vector<CVector> m_Stores;

    static CVector GetRandomHouse();
};