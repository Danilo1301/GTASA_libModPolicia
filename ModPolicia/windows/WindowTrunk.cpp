#include "WindowTrunk.h"

#include "Trunk.h"
#include "ModConfig.h"

#include "WindowCarMenu.h"

IWindow* WindowTrunk::m_Window = NULL;

extern IMenuVSL* menuVSL;

void WindowTrunk::Create()
{
	if (m_Window) return;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Position = ModConfig::MenuDefaultPosition;
    window->m_Title = "Trunk";

    auto button_editTrunk1 = window->AddButton("Edit trunk 0");
    button_editTrunk1->onClick = []() {
        Remove();
        CreateEditTrunk(0);
    };

    auto button_editTrunk2 = window->AddButton("Edit trunk 1");
    button_editTrunk2->onClick = []() {
        Remove();
        CreateEditTrunk(1);
    };

    auto button_close = window->AddButton(GetLanguageLine("close"));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowTrunk::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
}

void WindowTrunk::CreateEditTrunk(int trunkIndex)
{
    if (m_Window) return;

    auto vehicle = WindowCarMenu::m_Vehicle;
    auto trunkData = &Trunk::m_TrunkModels[vehicle->modelId];

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Position = ModConfig::MenuDefaultPosition;
    window->m_Title = "Trunk " + std::to_string(trunkIndex);

    auto button_position = window->AddButton("Edit position");
    button_position->onClick = [window, trunkData, vehicle, trunkIndex]() {
        Trunk::CreatePreviewPeds(vehicle);

        menuVSL->AddVectorWindowEx(window, &trunkData->trunkPositions[trunkIndex], -1000.0f, 1000.0f, 0.01f, []() {
            
            Trunk::UpdatePreviewPeds();

            menuVSL->ShowMessage("Updated position", 500);
        }, []() {
            
            Trunk::DestroyPreviewPeds();

            ModConfig::SaveSettings();

            menuVSL->ShowMessage("Saved!", 500);
        });
    };

    auto button_close = window->AddButton(GetLanguageLine("back"));
    button_close->onClick = []()
    {
        Remove();
        Create();
    };
}