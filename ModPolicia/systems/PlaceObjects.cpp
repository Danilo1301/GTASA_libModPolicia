#include "PlaceObjects.h"

#include "CleoFunctions.h"
#include "Input.h"
#include "Load.h"
#include "Log.h"

extern IMenuVSL* menuVSL;

int PlaceObjects::m_CarryingObject = -1;
IWindow* PlaceObjects::m_SelectionWindow = NULL;
IWindow* PlaceObjects::m_CarryWindow = NULL;
std::vector<int> PlaceObjects::m_PlacedObjects;
std::vector<int> PlaceObjects::m_AvailableObjects = {1237, 1238, 1228, 1459, 1434, 1427, 1949, 1425, 1422, 3091, 981, 1423, };

CVector displayObjectPosition = CVector(0, 0, 0);
int displayIndex = 0;
int displayObject = -1;

float objectPlaceHeight = 0;

void PlaceObjects::StartSelectingObjects()
{
    // 04C4: store_coords_to 1@ 2@ 3@ from_actor $PLAYER_ACTOR with_offset 0.0 0.0 5.0
    // 0160: set_camera_point_at 1@ 2@ 3@ switchstyle 1
    
    // 3@ -= 2.0
    
    // 0107: $Object = create_object 1237 at 1@ 2@ 3@
        
    // 04C4: store_coords_to 1@ 2@ 3@ from_actor $PLAYER_ACTOR with_offset 0.0 5.0 5.0
    // 015F: set_camera_position 1@ 2@ 3@ rotation 0.0 0.0 0.0

    float height = 3;

    displayObjectPosition = GetPedPositionWithOffset(GetPlayerActor(), CVector(0, 0, height));

    CleoFunctions::SET_CAMERA_POINT_AT(displayObjectPosition.x, displayObjectPosition.y, displayObjectPosition.z + 2, 1);

    CreateDisplayObject();

    auto cameraPos = displayObjectPosition + CVector(0, -5, 2);
    CleoFunctions::SET_CAMERA_POSITION(cameraPos.x, cameraPos.y, cameraPos.z, 0, 0, 0);

    CreateSelectionMenu();
}

void PlaceObjects::CreateSelectionMenu()
{
    if(m_SelectionWindow)
    {
        RemoveSelectionMenu();
    }

    auto window = m_SelectionWindow = menuVSL->AddWindow();
    window->m_Title = "";
    window->m_Position = CVector2D(0, 0);

    auto screenSize = Input::GetScreenSize();

    float x = screenSize.x/2;
    float y = 80;

    float buttonDistance = 100;
    float buttonW = 80.0f;
    float buttonWLarge = 110.0f;
    float buttonH = 50.0f;


    auto left = window->AddButton("<", CRGBA(255, 255, 255));
    left->m_FitInWindow = false;
    left->m_Position = CVector2D(x - buttonDistance, y);
    left->m_Size = CVector2D(buttonW, buttonH);
    left->onClick = []() {
 
        if (displayIndex > 0)
        {
            displayIndex--;
        }

        CreateSelectionMenu();
        CreateDisplayObject();
    };

    auto right = window->AddButton(">", CRGBA(255, 255, 255));
    right->m_FitInWindow = false;
    right->m_Position = CVector2D(x + buttonDistance, y);
    right->m_Size = CVector2D(buttonW, buttonH);
    right->onClick = []() {

        if (displayIndex < m_AvailableObjects.size() - 1)
        {
            displayIndex++;
        }

        CreateSelectionMenu();
        CreateDisplayObject();
    };

    auto cancel = window->AddButton("X", CRGBA(255, 0, 0));
    cancel->m_FitInWindow = false;
    cancel->m_Position = CVector2D(x - buttonDistance, y + 70);
    cancel->m_Size = CVector2D(buttonW, buttonH);
    cancel->onClick = []() {
        menuVSL->ShowMessage("Cancel", 2000);

        RemoveSelectionMenu();
        CleoFunctions::RESTORE_CAMERA_WITH_JUMPCUT();
        RemoveDisplayObject();
    };

    auto select = window->AddButton("Select", CRGBA(0, 173, 17));
    select->m_FitInWindow = false;
    select->m_Position = CVector2D(x + buttonDistance, y + 70);
    select->m_Size = CVector2D(buttonW, buttonH);
    select->onClick = []() {
        menuVSL->ShowMessage("Selected", 2000);

        RemoveSelectionMenu();
        StartCarryingObject(m_AvailableObjects[displayIndex]);
        CleoFunctions::RESTORE_CAMERA_WITH_JUMPCUT();
        RemoveDisplayObject();
    };

    auto removeAll = window->AddButton("Remove all", CRGBA(255, 106, 0));
    removeAll->m_FitInWindow = false;
    removeAll->m_Position = CVector2D(x + buttonDistance + 100, y + 70);
    removeAll->m_Size = CVector2D(buttonWLarge, buttonH);
    removeAll->onClick = []() {
        menuVSL->ShowMessage("Remove all", 2000);

        RemoveSelectionMenu();
        RemoveAllObjects();
        CleoFunctions::RESTORE_CAMERA_WITH_JUMPCUT();
        RemoveDisplayObject();
    };
}

void PlaceObjects::RemoveSelectionMenu()
{
    m_SelectionWindow->SetToBeRemoved();
    m_SelectionWindow = NULL;
}

void PlaceObjects::CreateDisplayObject()
{
    if(displayObject != -1)
    {
        RemoveDisplayObject();
    }

    displayObject = CleoFunctions::CREATE_OBJECT(m_AvailableObjects[displayIndex], displayObjectPosition.x, displayObjectPosition.y, displayObjectPosition.z);

    menuVSL->ShowMessage("ID " + std::to_string(m_AvailableObjects[displayIndex]), 2000);
}

void PlaceObjects::RemoveDisplayObject()
{
    CleoFunctions::DESTROY_OBJECT(displayObject);
    displayObject = -1;
}

void PlaceObjects::StartCarryingObject(int objectId)
{
    menuVSL->debug->AddLine("carrying object " + std::to_string(objectId));

    auto position = GetPedPositionWithOffset(GetPlayerActor(), CVector(0, 2, 0));

    m_CarryingObject = CleoFunctions::CREATE_OBJECT(objectId, position.x, position.y, position.z);

    SetPlaceWindowEnabled(true);
}

void PlaceObjects::StopCarryingObject()
{
    m_PlacedObjects.push_back(m_CarryingObject);
    m_CarryingObject = -1;

    menuVSL->ShowMessage("Voce colocou " + std::to_string(m_PlacedObjects.size()) + " objetos", 2000);

    menuVSL->debug->AddLine("you placed " + std::to_string(m_PlacedObjects.size()) + " objects");
}

void PlaceObjects::Update(int dt)
{
    if(m_CarryingObject != -1)
    {
        auto object = m_CarryingObject;

        auto pedPosition = GetPedPositionWithOffset(GetPlayerActor(), CVector(0, 2, 0));

        auto ground = CleoFunctions::GROUND_Z_AT(pedPosition.x, pedPosition.y, pedPosition.z);

        ground += 0.5f;
    
        CleoFunctions::PUT_OBJECT_AT(object, pedPosition.x, pedPosition.y, ground + objectPlaceHeight);

        auto heading = CleoFunctions::ACTOR_Z_ANGLE(GetPlayerActor());

        CleoFunctions::SET_OBJECT_Z_ANGLE(object, heading);
    }
}

void PlaceObjects::SetPlaceWindowEnabled(bool enabled)
{
    if(enabled)
    {
        if(m_CarryWindow != NULL) return;

        auto window = m_CarryWindow = menuVSL->AddWindow();
        window->m_Title = "";
        window->m_Position = CVector2D(0, 0);

        auto screenSize = Input::GetScreenSize();
        float w = 200.0f;
        float x = screenSize.x/2 - w/2;
        float y = 80;

        auto stopCarry_button = window->AddButton("OK", CRGBA(60, 130, 180));
        stopCarry_button->m_FitInWindow = false;
        stopCarry_button->m_Position = CVector2D(x, y);
        stopCarry_button->m_Size = CVector2D(200, 50);
        stopCarry_button->onClick = []() {
            StopCarryingObject();
            SetPlaceWindowEnabled(false);
        };

        auto goUp = window->AddButton("+", CRGBA(60, 130, 180));
        goUp->m_FitInWindow = false;
        goUp->m_Position = CVector2D(x, y + 80);
        goUp->m_Size = CVector2D(50, 50);
        goUp->onClick = []() {
            objectPlaceHeight += 0.1f;
        };

        auto goDown = window->AddButton("-", CRGBA(60, 130, 180));
        goDown->m_FitInWindow = false;
        goDown->m_Position = CVector2D(x + 60, y + 80);
        goDown->m_Size = CVector2D(50, 50);
        goDown->onClick = []() {
            objectPlaceHeight -= 0.1f;
        };
    } else {
        
        if(m_CarryWindow == NULL) return;

        m_CarryWindow->SetToBeRemoved();
        m_CarryWindow = NULL;
    }
}

void PlaceObjects::RemoveAllObjects()
{
    for(auto object : m_PlacedObjects)
    {
        CleoFunctions::DESTROY_OBJECT(object);
    }
    m_PlacedObjects.clear();
}