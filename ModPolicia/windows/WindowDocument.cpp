#include "WindowDocument.h"

#include "Log.h"

Window* WindowDocument::m_Window = NULL;
DOC_TYPE WindowDocument::m_DocumentType = DOC_TYPE::RG;
Ped* WindowDocument::m_Ped = NULL;
std::function<void()> WindowDocument::m_OnClose;

CVector2D WindowDocument::m_TestPosition = CVector2D(535, 370);

void WindowDocument::Create()
{
    auto window = m_Window = Menu::AddWindow(6);
    
    auto confirm_button = window->AddFloatingButton(7, 0,0, CVector2D(660, 120), CVector2D(100, 30));
    confirm_button->onClick = []()
    {
        Remove();

        if(m_OnClose)
        {
            m_OnClose();
            m_OnClose = NULL;
        }
    };
}

void WindowDocument::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}

void WindowDocument::ToggleDocuments(DOC_TYPE documentType, Ped* ped)
{
    m_DocumentType = documentType;
    m_Ped = ped;

    if(m_Window)
    {
        Remove();
        return;
    }

    Create();
}

void WindowDocument::Draw()
{
    if(!m_Window) return;

    if(!m_Ped) return;

    if(m_DocumentType == DOC_TYPE::RG)
    {

        //Fundo RG
        Draw::DrawSprite(100, CVector2D(200, 120), CVector2D(430, 275), CRGBA(150, 150, 150));

        //TEST text
        Draw::DrawText(2, 999, 999, m_TestPosition, CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Registro Geral
        Draw::DrawText(40, m_Ped->rg[0], m_Ped->rg[1], CVector2D(284, 152), CRGBA(110, 0, 0), eTextAlign::ALIGN_LEFT);

        //ID
        Draw::DrawText(27, m_Ped->hPed, 0, CVector2D(491, 155), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
        
        //Nome
        Draw::DrawText(39, 0, 0, CVector2D(365, 241), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Data de nascimento
        Draw::DrawText(42, m_Ped->birthDay, m_Ped->birthMonth, CVector2D(365, 288), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
        Draw::DrawText(43, m_Ped->birthYear, 0, CVector2D(420, 288), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //CPF
        Draw::DrawText(41, m_Ped->cpf[0], m_Ped->cpf[1], CVector2D(365, 336), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
    } else {
        //Fundo CNH
        Draw::DrawSprite(101, CVector2D(200, 120), CVector2D(430, 275), CRGBA(150, 150, 150));

        //TEST text
        Draw::DrawText(2, 999, 999, m_TestPosition, CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Nome
        Draw::DrawText(39, 0, 0, CVector2D(276, 200), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //ID
        Draw::DrawText(1, m_Ped->hPed, 0, CVector2D(412, 247), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Nº de Registro
        Draw::DrawText(1, m_Ped->cnhRegisterNum, 0, CVector2D(279, 367), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Validade
        Draw::DrawText(42, m_Ped->cnhValidDay, m_Ped->cnhValidMonth, CVector2D(426, 367), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
        Draw::DrawText(43, m_Ped->cnhValidYear, 0, CVector2D(480, 367), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
    }

    /*
    if(m_DocumentType == DOC_TYPE::RG)
    {
        //Fundo RG
        Draw::DrawSprite(1, CVector2D(200, 120), CVector2D(430, 275), CRGBA(150, 150, 150));

        //Test text
        Draw::DrawText(2, 555, 369, m_TestPosition, CRGBA(0, 0, 0));

        //

        //Registro Geral
        Draw::DrawText(40, 495, 369, CVector2D(310, 150), CRGBA(120, 0, 0));
        
        //ID
        //Draw::DrawText(2, 495, 369, CVector2D(310, 150), CRGBA(180, 0, 0));
        
        //Nome
        Draw::DrawText(39, 495, 369, CVector2D(400, 237), CRGBA(180, 0, 0));

        //Data de nascimento
        Draw::DrawText(42, 25, 9, CVector2D(400, 287), CRGBA(180, 0, 0));

        //CPF
        Draw::DrawText(41, 495, 369, CVector2D(400, 325), CRGBA(180, 0, 0));
    } else if (m_DocumentType == DOC_TYPE::CNH) {
        //Fundo CNH
        Draw::DrawSprite(2, CVector2D(200, 120), CVector2D(430, 275), CRGBA(150, 150, 150));
    }
    */

   Log::file << "6" << std::endl;
}