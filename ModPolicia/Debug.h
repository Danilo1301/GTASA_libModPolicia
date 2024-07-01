#pragma once

#include "pch.h"

struct DebugLine {
    int gxtId = 0;
    int num1 = 0;
    int num2 = 0;
    CRGBA color = {255, 255, 0, 255};
};

class Debug {
public:
    static std::vector<DebugLine> m_Lines;

    static void Draw();

    static void AddLine(int gxtId, int num1);
    static void AddLine(int gxtId, int num1, CRGBA color);
    static void AddLine(int gxtId, int num1, int num2);
    static void AddLine(int gxtId, int num1, int num2, CRGBA color);
};