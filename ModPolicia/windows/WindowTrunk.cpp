#include "WindowTrunk.h"

#include "Trunk.h"
#include "ModConfig.h"

#include "WindowCarMenu.h"

Window* WindowTrunk::m_Window = NULL;

void WindowTrunk::Create()
{
	if (m_Window) return;

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto button_editTrunk1 = window->AddButton(166, 1, 0);
    button_editTrunk1->onClick = []() {
        Remove();
        CreateEditTrunk(0);
    };

    auto button_editTrunk2 = window->AddButton(166, 2, 0);
    button_editTrunk2->onClick = []() {
        Remove();
        CreateEditTrunk(1);
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowTrunk::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}

void WindowTrunk::CreateEditTrunk(int trunkIndex)
{
    if (m_Window) return;

    auto vehicle = WindowCarMenu::m_Vehicle;
    auto trunkData = &Trunk::m_TrunkModels[vehicle->modelId];

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto button_position = window->AddButton(9, 1, 0);
    button_position->onClick = [window, trunkData, vehicle, trunkIndex]() {
        Trunk::CreatePreviewPeds(vehicle);

        auto positionWindow = Menu::AddPositionWindow(window, &trunkData->trunkPositions[trunkIndex], -1000.0f, 1000.0f, 0.01f, []() {
            Trunk::UpdatePreviewPeds();
        }, []() {
            Trunk::DestroyPreviewPeds();

            ModConfig::SaveSettings();
        });
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        Create();
    };
}