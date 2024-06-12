#include "PoliceDepartment.h"

#include "windows/WindowPD_Menu.h"

Pickup* PoliceDepartment::m_PickupMenu = new Pickup(CVector(253.4084, 68.5503, 1003.6406), 1210);
Pickup* PoliceDepartment::m_PickupEquipment = new Pickup(CVector(253.7876, 76.5393, 1003.6406), 1242);

void PoliceDepartment::Init()
{

}

void PoliceDepartment::Update(int dt)
{
    m_PickupMenu->Update(dt);
    m_PickupEquipment->Update(dt);

    if(m_PickupMenu->playerJustPickedPickup)
    {
        WindowPD_Menu::Create();
    }

    if(m_PickupEquipment->playerJustPickedPickup)
    {
        
    }
}