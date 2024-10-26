#include "WindowTicket.h"

#include "WindowPullover.h"

#include "../CleoFunctions.h"

extern IMenuVSL* menuVSL;

IWindow* WindowTicket::m_Window = NULL;

void WindowTicket::Create()
{
    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = GetLanguageLine("window_ticket");

    for(int i = 1; i <= 4; i++)
    {
        auto button = window->AddButton(GetLanguageLine("ticket" + std::to_string(i)));
        button->onClick = []()
        {
            IssueTicket(0);
        };
    }

    auto button_close = window->AddButton(GetLanguageLine("back"));
    button_close->onClick = []()
    {
        Remove();
        WindowPullover::CreatePullingPed();
    };
}

void WindowTicket::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
}

void WindowTicket::IssueTicket(int price)
{
    Remove();
    WindowPullover::CreatePullingPed();

    menuVSL->ShowMessage(GetLanguageLine("ticket_issued"), 3000);
}