#include "WindowTicket.h"

#include "WindowPullover.h"

#include "../CleoFunctions.h"

Window* WindowTicket::m_Window = NULL;

void WindowTicket::Create()
{
    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    auto ticket1 = window->AddButton(100);
    ticket1->onClick = []()
    {
        IssueTicket(0);
    };

    auto ticket2 = window->AddButton(101);
    ticket2->onClick = []()
    {
        IssueTicket(0);
    };

    auto ticket3 = window->AddButton(102);
    ticket3->onClick = []()
    {
        IssueTicket(0);
    };

    auto ticket4 = window->AddButton(103);
    ticket4->onClick = []()
    {
        IssueTicket(0);
    };

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
        WindowPullover::CreatePullingPed();
    };
}

void WindowTicket::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}

void WindowTicket::IssueTicket(int price)
{
    Remove();
    WindowPullover::CreatePullingPed();
    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX104", 0, 0, 0, 3000, 1); //multa aplicada
}