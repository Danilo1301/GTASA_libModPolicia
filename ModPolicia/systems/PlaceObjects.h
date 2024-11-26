#pragma once

#include "pch.h"
#include "menu/IMenuVSL.h"

class PlaceObjects {
public:
    static int m_CarryingObject;
    static IWindow* m_SelectionWindow;
    static IWindow* m_CarryWindow;
    static std::vector<int> m_PlacedObjects;
    static std::vector<int> m_AvailableObjects;

    static void StartSelectingObjects();

private:
    static void CreateSelectionMenu();
    static void RemoveSelectionMenu();
    static void CreateDisplayObject();

    static void StartCarryingObject(int objectId);
    static void StopCarryingObject();

public:
    static void Update(int dt);

private:
    static void SetPlaceWindowEnabled(bool enabled);

public:
    static void RemoveAllObjects();
};