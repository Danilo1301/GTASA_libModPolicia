#pragma once

#include <map>
#include <string>
#include <vector>

#include "SimpleGTA.h"

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