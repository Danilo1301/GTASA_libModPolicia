#include "WindowFrisk.h"

#include "WindowPullover.h"

#include "Pullover.h"
#include "Log.h"
#include "CleoFunctions.h"
#include "SoundSystem.h"

Window* WindowFrisk::m_Window = NULL;
Window* WindowFrisk::m_WindowItemActions = NULL;

void WindowFrisk::Create()
{
    auto ped = Pullover::m_PullingPed;

    auto window = m_Window = Menu::AddWindow(6);
    window->position = CVector2D(200, 200); //80, 200
    window->showPageControls = true;
    
    for(auto item : ped->inventory->items)
    {
        auto button = window->AddButton(item->itemNameGxtId, 0, 0);
        button->onClick = [item]()
        {
            Remove();
            CreateItemActions(item, []() {
                Create();
            });
        };
    }

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Pullover::m_FriskType = FRISK_TYPE::FRISK_NONE;
        Remove();
        WindowPullover::CreatePullingPed();
    };
}

void WindowFrisk::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}

void WindowFrisk::CreateItemActions(InventoryItem* item, std::function<void()> onClose)
{
    auto ped = Pullover::m_PullingPed;
    auto vehicle = Pullover::m_PullingVehicle;

    auto window = m_WindowItemActions = Menu::AddWindow(6);
    window->position = CVector2D(200, 200); //80, 200
    window->showPageControls = true;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "item " << item->itemNameGxtId << " canBeAprehended: " << (item->canBeAprehended ? "TRUE" : "FALSE") << std::endl;
    
    if(item->type == Item_Type::DOCUMENTS)
    {
        window->AddText(113, ped->money, 0, CRGBA(255, 255, 255));
    }

    if(item->type == Item_Type::CELLPHONE)
    {
        auto button_imei = window->AddButton(50);
        button_imei->onClick = [item]()
        {   
            SoundSystem::PlayHTAudio();

            if(item->isStolen) {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX53", 0, 0, 0, 3000, 1); //furto
            } else {
                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX54", 0, 0, 0, 3000, 1); //nada consta
            }
        };
    }

    if(item->canBeAprehended)
    {
        auto button_apreender = window->AddButton(51);
        button_apreender->onClick = [ped, vehicle, item, onClose]()
        {
            if(Pullover::m_FriskType == FRISK_TYPE::FRISK_PED) ped->inventory->RemoveItemFromInventory(item);
            else vehicle->inventory->RemoveItemFromInventory(item);

            RemoveItemActions();
            onClose();
        };
    }

    if(item->type == Item_Type::WEED)
    {
        auto button_fumar = window->AddButton(52);
        button_fumar->onClick = [ped, vehicle, item, onClose]()
        {
            if(Pullover::m_FriskType == FRISK_TYPE::FRISK_PED) ped->inventory->RemoveItemFromInventory(item);
            else vehicle->inventory->RemoveItemFromInventory(item);

            ped->inventory->RemoveItemFromInventory(item);
            RemoveItemActions();
            onClose();
        };
    }

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = [onClose]()
    {
        RemoveItemActions();
        onClose();
    };
}


void WindowFrisk::RemoveItemActions()
{
    m_WindowItemActions->RemoveThisWindow();
    m_WindowItemActions = NULL;
}

void WindowFrisk::CreateFriskCar()
{
    auto vehicle = Pullover::m_PullingVehicle;

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    for(auto item : vehicle->inventory->items)
    {
        auto button = window->AddButton(item->itemNameGxtId, 0, 0);
        button->onClick = [item]()
        {
            Remove();
            WindowFrisk::CreateItemActions(item, []() {
                CreateFriskCar();
            });
        };
    }

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Pullover::m_FriskType = FRISK_TYPE::FRISK_NONE;
        Remove();
        WindowPullover::CreatePullingPed();
    };
}