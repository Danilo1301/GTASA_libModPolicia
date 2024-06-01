#pragma once

#include "pch.h"

#include "SimpleGTA.h"

enum CITY {
    LOS_SANTOS,
    LAS_VENTURAS,
    SAN_FIERRO
};

/*
CVector position;
int angle;
CITY city;
*/
struct Location {
    CVector position;
    int angle;
    CITY city;
};

class Locations {
public:
    static std::vector<Location> m_Houses;
    static std::vector<Location> m_Stores;
    static std::vector<Location> m_PoliceDepartments;

    static CVector GetRandomHouse();

    static CITY GetClosestCity();

    static Location GetClosestPoliceDepartment();
};