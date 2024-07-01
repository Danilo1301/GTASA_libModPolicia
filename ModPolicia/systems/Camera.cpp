#include "Camera.h"

#include "menu/Menu.h"

CVector2D Camera::m_Position = CVector2D(500, 10);
CVector2D Camera::m_Size = CVector2D(330, 70);
bool Camera::m_Enabled = false;

void Camera::Draw()
{
    if(!m_Enabled) return;

    Draw::DrawSprite(124, m_Position, m_Size, CRGBA(255, 255, 255));
}