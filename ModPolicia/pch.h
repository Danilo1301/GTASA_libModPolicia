#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <dlfcn.h>
//#include <windows.h>
#include <sys/stat.h>
#include <ctype.h>

#include "SimpleGTA.h"

#include "json/json.h"

#include "ibass.h"

static int GetRandomNumber(int min, int max)
{
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do{
        x = rand();
    }while (x >= RAND_MAX - remainder);
    return min + x % n;
}

static double distanceBetweenTwoPoints(double x, double y, double a, double b)
{
    return sqrt(pow(x - a, 2) + pow(y - b, 2));
}

static double DistanceBetweenPoints(CVector point1, CVector point2)
{
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;
    double dz = point1.z - point2.z;

    return sqrt( dx * dx + dy * dy + dz * dz );
}

static std::string to_upper(std::string data) {
    std::for_each(data.begin(), data.end(), [](char& c) {
        c = ::toupper(c);
    });
    return data;
}

static std::string to_lower(std::string data) {
    std::for_each(data.begin(), data.end(), [](char& c) {
        c = ::tolower(c);
    });
    return data;
}

static std::string CVectorToString(CVector vec)
{
    return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}

static std::string CVector2DToString(CVector2D vec)
{
    return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}