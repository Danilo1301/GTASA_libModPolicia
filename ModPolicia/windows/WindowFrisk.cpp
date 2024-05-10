#include "WindowFrisk.h"

#include "WindowTest.h"

#include "../Pullover.h"
#include "../CleoFunctions.h"

Window* WindowFrisk::m_Window = NULL;
Window* WindowFrisk::m_WindowItemActions = NULL;

void WindowFrisk::Create()
{
    auto ped = Pullover::m_PullingPed;

    auto window = m_Window = Menu::AddWindow(6);
    window->position = CVector2D(200, 200); //80, 200
    window->showPageControls = true;
    
    for(auto item : ped->inventoryItems)
    {
        auto button = window->AddButton(item->itemNameGxtId, 0, 0);
        button->onClick = [item]()
        {
            Remove();
            CreateItemActions(item);
        };
    }

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        WindowTest::Create();
    };
}

void WindowFrisk::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}

void WindowFrisk::CreateItemActions(InventoryItem* item)
{
    auto ped = Pullover::m_PullingPed;

    auto window = m_WindowItemActions = Menu::AddWindow(6);
    window->position = CVector2D(200, 200); //80, 200
    window->showPageControls = true;

    if(item->type == Item_Type::CELLPHONE)
    {
        auto button_imei = window->AddButton(50);
        button_imei->onClick = [item]()
        {
            if(item->isSlotenCellphone) {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX53", 0, 0, 0, 3000, 1); //furto
            } else {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX54", 0, 0, 0, 3000, 1); //nada consta
            }
        };

        auto button_apreender = window->AddButton(51);
        button_apreender->onClick = [ped, item]()
        {
            ped->RemoveItemFromInventory(item);
            RemoveItemActions();
            Create();
        };
    }

    if(item->type == Item_Type::WEED)
    {
        auto button_apreender = window->AddButton(51);
        button_apreender->onClick = [ped, item]()
        {
            ped->RemoveItemFromInventory(item);
            RemoveItemActions();
            Create();
        };

        auto button_fumar = window->AddButton(52);
        button_fumar->onClick = [ped, item]()
        {
            ped->RemoveItemFromInventory(item);
            RemoveItemActions();
            Create();
        };
    }

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        RemoveItemActions();
        Create();
    };
}

void WindowFrisk::RemoveItemActions()
{
    m_WindowItemActions->RemoveThisWindow();
    m_WindowItemActions = NULL;
}