#include "WindowFrisk.h"

#include "WindowPullover.h"

#include "Pullover.h"
#include "Log.h"
#include "CleoFunctions.h"
#include "SoundSystem.h"

IWindow* WindowFrisk::m_Window = NULL;
IWindow* WindowFrisk::m_WindowItemActions = NULL;

extern IMenuVSL* menuVSL;

void WindowFrisk::Create()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Criando revista pessoal" << std::endl;
    
    auto ped = Pullover::m_PullingPed;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "Revista pessoal";
    
    for(auto item : ped->inventory->items)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "item " << item->name << " canBeAprehended: " << (item->canBeAprehended ? "TRUE" : "FALSE") << std::endl;

        auto button = window->AddButton(item->name);
        button->onClick = [item]()
        {
            Remove();
            CreateItemActions(item, []() {
                Create();
            });
        };
        
        button->m_StringAtRight = InventoryItems::FormatItemAmount(item);
    }

    auto button_close = window->AddButton("~r~Finalizar revista");
    button_close->onClick = []()
    {
        Pullover::m_FriskType = FRISK_TYPE::FRISK_NONE;
        Remove();
        WindowPullover::CreatePullingPed();
    };
}

void WindowFrisk::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
}

void WindowFrisk::CreateItemActions(InventoryItem* item, std::function<void()> onClose)
{
    auto ped = Pullover::m_PullingPed;
    auto vehicle = Pullover::m_PullingVehicle;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "Revista (Item)";

    Log::Level(LOG_LEVEL::LOG_BOTH) << "item " << item->name << " canBeAprehended: " << (item->canBeAprehended ? "TRUE" : "FALSE") << std::endl;
    
    if(item->type == Item_Type::DOCUMENTS)
    {
        std::string moneyText = "Dinheiro: " + std::to_string(ped->money) + "$";

        window->AddText(moneyText, CRGBA(255, 255, 255));
    }

    if(item->type == Item_Type::CELLPHONE)
    {
        auto button_imei = window->AddButton("Consultar IMEI");
        button_imei->onClick = [item]()
        {   
            SoundSystem::PlayHTAudio();
            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/CHECK_IMEI_", false);
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX217", 0, 0, 0, 3000, 1); //consultar imei

            CleoFunctions::WAIT(4000, [item]() {
                if(item->isStolen) {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/IMEI_STOLEN_", false);
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX53", 0, 0, 0, 3000, 1); //furto
                } else {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/IMEI_OK_", false);
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX54", 0, 0, 0, 3000, 1); //nada consta
                }
            });
        };
    }

    if(item->canBeAprehended)
    {
        auto button_apreender = window->AddButton("Apreender");
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
        auto button_fumar = window->AddButton("Fumar");
        button_fumar->onClick = [ped, vehicle, item, onClose]()
        {
            if(Pullover::m_FriskType == FRISK_TYPE::FRISK_PED) ped->inventory->RemoveItemFromInventory(item);
            else vehicle->inventory->RemoveItemFromInventory(item);

            ped->inventory->RemoveItemFromInventory(item);
            RemoveItemActions();
            onClose();
        };
    }

    auto button_close = window->AddButton("~r~Voltar");
    button_close->onClick = [onClose]()
    {
        RemoveItemActions();
        onClose();
    };
}


void WindowFrisk::RemoveItemActions()
{
    m_WindowItemActions->SetToBeRemoved();
    m_WindowItemActions = NULL;
}

void WindowFrisk::CreateFriskCar()
{
    auto vehicle = Pullover::m_PullingVehicle;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "Revista veicular";

    for(auto item : vehicle->inventory->items)
    {
        auto button = window->AddButton(item->name);
        button->onClick = [item]()
        {
            Remove();
            WindowFrisk::CreateItemActions(item, []() {
                CreateFriskCar();
            });
        };
        
        button->m_StringAtRight = InventoryItems::FormatItemAmount(item);
    }

     auto button_close = window->AddButton("~r~Finalizar revista");
    button_close->onClick = []()
    {
        Pullover::m_FriskType = FRISK_TYPE::FRISK_NONE;
        Remove();
        WindowPullover::CreatePullingPed();
    };
}