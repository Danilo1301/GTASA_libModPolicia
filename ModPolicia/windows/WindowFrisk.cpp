#include "WindowFrisk.h"

#include "WindowPullover.h"
#include "WindowCarMenu.h"

#include "Pullover.h"
#include "Log.h"
#include "CleoFunctions.h"
#include "SoundSystem.h"
#include "ModConfig.h"

IWindow* WindowFrisk::m_Window = NULL;
IWindow* WindowFrisk::m_WindowItemActions = NULL;

extern IMenuVSL* menuVSL;

void WindowFrisk::Create()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Criando revista pessoal" << std::endl;
    
    auto ped = Pullover::m_PullingPed;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Position = ModConfig::MenuDefaultPosition;
    window->m_Title = GetLanguageLine("frisk_ped");
    
    for(auto item : ped->inventory->items)
    {
        auto button = window->AddButton(item->itemData->name);
        button->onClick = [item]()
        {
            Remove();
            CreateItemActions(item, []() {
                Create();
            });
        };
        
        button->m_StringAtRight = InventoryItems::FormatItemAmount(item);
    }

    auto button_close = window->AddButton(GetLanguageLine("finish_frisk"));
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
    auto vehicle = WindowCarMenu::m_Vehicle;

    auto window = m_WindowItemActions = menuVSL->AddWindow();
    window->m_Position = ModConfig::MenuDefaultPosition;
    window->m_Title = GetLanguageLine("frisk_item");
    
    if(item->itemData->type == ItemType::DOCUMMENTS)
    {
        std::string moneyText = GetLanguageLine("wallet_money", ped->money);

        window->AddText(moneyText, CRGBA(255, 255, 255));
    }

    if(item->itemData->type == ItemType::CELLPHONE)
    {
        auto button_imei = window->AddButton(GetLanguageLine("check_imei"));
        button_imei->onClick = [item]()
        {   
            SoundSystem::PlayHTAudio();
            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/CHECK_IMEI_", false);

            menuVSL->ShowMessage(GetLanguageLine("voice_check_imei"), 3000);

            CleoFunctions::WAIT(4000, [item]() {
                if(item->isStolen) {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/IMEI_STOLEN_", false);
                    
                    menuVSL->ShowMessage(GetLanguageLine("voice_imei_stolen"), 3000);
                } else {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/IMEI_OK_", false);
                    
                    menuVSL->ShowMessage(GetLanguageLine("voice_imei_ok"), 3000);
                }
            });
        };
    }

    if(item->itemData->canConfiscate)
    {
        auto button_apreender = window->AddButton(GetLanguageLine("seize"));
        button_apreender->onClick = [ped, vehicle, item, onClose]()
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Apreender" << std::endl;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Remover " << item << " do inventario " << std::endl;

            if(Pullover::m_FriskType == FRISK_TYPE::FRISK_PED) ped->inventory->RemoveItemFromInventory(item);
            else vehicle->inventory->RemoveItemFromInventory(item);

            Log::Level(LOG_LEVEL::LOG_BOTH) << "RemoveItemActions()" << std::endl;

            RemoveItemActions();

            Log::Level(LOG_LEVEL::LOG_BOTH) << "onClose()" << std::endl;

            onClose();
        };
    }

    if(item->itemData->type == ItemType::ILEGAL_DRUG)
    {
        auto button_fumar = window->AddButton(GetLanguageLine("smoke"));
        button_fumar->onClick = [ped, vehicle, item, onClose]()
        {
            if(Pullover::m_FriskType == FRISK_TYPE::FRISK_PED) ped->inventory->RemoveItemFromInventory(item);
            else vehicle->inventory->RemoveItemFromInventory(item);

            RemoveItemActions();
            onClose();
        };
    }

    auto button_close = window->AddButton(GetLanguageLine("back"));
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

void WindowFrisk::CreateFriskCar(Vehicle* vehicle)
{
    auto window = m_Window = menuVSL->AddWindow();
    window->m_Position = ModConfig::MenuDefaultPosition;
    window->m_Title = GetLanguageLine("frisk_vehicle");

    for(auto item : vehicle->inventory->items)
    {
        auto button = window->AddButton(item->itemData->name);
        button->onClick = [item, vehicle]()
        {
            Remove();
            CreateItemActions(item, [vehicle]() {
                WindowFrisk::CreateFriskCar(vehicle);
            });
        };
        
        button->m_StringAtRight = InventoryItems::FormatItemAmount(item);
    }

    auto button_close = window->AddButton(GetLanguageLine("finish_frisk"));
    button_close->onClick = [vehicle]()
    {
        Pullover::m_FriskType = FRISK_TYPE::FRISK_NONE;
        Remove();
        WindowCarMenu::Create(vehicle);
    };
}