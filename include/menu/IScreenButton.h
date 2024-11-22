#pragma once

#include <iostream>

#include "SimpleGTA.h"

#include "IItem.h"

class IScreenButton {
public:
    CVector2D m_Position = CVector2D(200, 200);
    CVector2D m_Size = CVector2D(50, 50);
    std::string m_Text = "";

    std::function<void()> onClick;

    virtual void SetToBeRemoved() = 0;
};