#pragma once

#include "pch.h"

class Camera {
public:
    static CVector2D m_Position;
    static CVector2D m_Size;
    static bool m_Enabled;

    static void Draw();
};