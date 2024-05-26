#include "Peds.h"

#include "SimpleGTA.h"

#include "Log.h"

extern uintptr_t* pPedPool;
extern int (*GetPedRef)(int);

std::map<int, Ped*> Peds::m_Peds;

void Peds::Update(int dt)
{
    TryFindNewPeds();

    for(auto pair : m_Peds)
    {
        auto ped = pair.second;

        ped->Update(dt);
    }
}

void Peds::TryFindNewPeds()
{
    //GET_RANDOM_CHAR_IN_SPHERE_NO_SAVE_RECURSIVE
    //https://github.com/AndroidModLoader/GTA_CLEOMod/blob/RUSJJ_CLEO_MOD/cleo4opcodes.cpp

    /*
    it fucking crashes ;-; WHY... I WANTED THIS FUNCTION SO MUCH

    auto objects = *(GTAEntity**)(*pPedPool + 0);
    tByteFlag* flags = *(tByteFlag**)(*pPedPool + 4);
    int size = *(int*)(*pPedPool + 8);

    Log::file << "for loop, size: " << size << std::endl;

    for (int i = 0; i < size; ++i)
    {
        if (flags[i].bEmpty) continue;

        Log::file << "i= " << i << std::endl;

        auto& ent = objects[i];

        int ref = GetPedRef(ent.AsInt());

        Log::file << "Found ped: " << ent.AsInt() << ", " << &ent << std::endl;

        if(Peds::HasPedHandle(ref))
        {
            continue;
        }

        Log::file << "ref " << ref << ", " << std::endl;
        
        Peds::TryCreatePed(ref);
        
    }

    Log::file << "end for" << std::endl;*/

}

bool Peds::HasPedHandle(int hPed)
{
    return m_Peds.find(hPed) != m_Peds.end();
}

Ped* Peds::TryCreatePed(int hPed)
{
	if (HasPedHandle(hPed))
    {
        return GetPedByHandle(hPed);
    }

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Peds: Add ped " << hPed << "(" << std::to_string(m_Peds.size() + 1) << " total)" << std::endl;

    auto ped = new Ped(hPed);

	m_Peds[hPed] = ped;

    return ped;
}

Ped* Peds::GetPedByHandle(int hPed)
{
	if (!HasPedHandle(hPed)) return NULL;
	return m_Peds.at(hPed);
}