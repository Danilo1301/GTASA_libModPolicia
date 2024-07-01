#pragma once

#include "pch.h"

struct DebugLine {
    int gxtId = 0;
    int num1 = 0;
    int num2 = 0;
};

class Debug {
public:
    static std::vector<DebugLine> m_Lines;

    static void Draw();

    static void AddLine(int gxtId, int num1);
    static void AddLine(int gxtId, int num1, int num2);
};