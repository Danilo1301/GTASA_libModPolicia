#include "Base.h"

Base::Base()
{
    m_PickupDuty = new Pickup(CVector(0, 0, 0), 1210);
    m_PickupEquipment = new Pickup(CVector(0, 0, 0), 1242);
    m_PickupPartner = new Pickup(CVector(0, 0, 0), 1314);
}