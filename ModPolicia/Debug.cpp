#include "Debug.h"

#include "menu/Menu.h"

std::vector<DebugLine> Debug::m_Lines;

void Debug::Draw()
{
    float x = 20;
    float y = 200;

    for(auto line : m_Lines)
    {
       Draw::DrawGxtText(line.gxtId, line.num1, line.num2, CVector2D(x, y), line.color, eTextAlign::ALIGN_LEFT);

       y += 20;
    }
}
    
void Debug::AddLine(int gxtId, int num1)
{
    AddLine(gxtId, num1, 0);
}

void Debug::AddLine(int gxtId, int num1, CRGBA color)
{
    AddLine(gxtId, num1, 0, color);
}

void Debug::AddLine(int gxtId, int num1, int num2)
{
    AddLine(gxtId, num1, num2, CRGBA(255, 255, 255));
}   

void Debug::AddLine(int gxtId, int num1, int num2, CRGBA color)
{
    DebugLine line;
    line.gxtId = gxtId;
    line.num1 = num1;
    line.num2 = num2;
    line.color = color;

    m_Lines.push_back(line);

    if (m_Lines.size() >= 10) m_Lines.erase(m_Lines.begin());
}