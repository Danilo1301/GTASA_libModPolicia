#include "WindowDocument.h"

#include "Log.h"
#include "CleoFunctions.h"
#include "Pullover.h"
#include "SoundSystem.h"

Window* WindowDocument::m_Window = NULL;
DOC_TYPE WindowDocument::m_DocumentType = DOC_TYPE::RG;
Ped* WindowDocument::m_Ped = NULL;
std::function<void()> WindowDocument::m_OnClose;

CVector2D WindowDocument::m_TestPosition = CVector2D(548, 404);

void WindowDocument::Create()
{
    auto window = m_Window = Menu::AddWindow(6);
    window->position = CVector2D(0, 0);
    
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

    if(m_DocumentType == DOC_TYPE::RG)
    {
        auto confirm_consultarrg = window->AddFloatingButton(66, 0,0, CVector2D(660, 160), CVector2D(100, 30));
        confirm_consultarrg->onClick = []()
        {
            Remove();

            SoundSystem::PlayHTAudio();
            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/CHECK_ID_", false);
            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX71", 0, 0, 0, 3000, 1); //consultar rg

            CleoFunctions::WAIT(4000, []() {
                auto ped = m_Ped;
                if(ped->isWanted)
                {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/ID_WITH_ARREST_WARRANT_", false);
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX73", 0, 0, 0, 3000, 1); //com mandado
                } else {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/ID_OK_", false);
                    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX72", 0, 0, 0, 3000, 1); //sem queixas
                }

                CleoFunctions::WAIT(1000, []() {
                    Create();
                });
            });
        };
    }
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
        Draw::DrawSprite(101, CVector2D(200, 120), CVector2D(430, 275), CRGBA(150, 150, 150));

        //TEST text
        Draw::DrawGxtText(2, 999, 999, m_TestPosition, CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Registro Geral
        Draw::DrawGxtText(40, m_Ped->rg[0], m_Ped->rg[1], CVector2D(284, 152), CRGBA(110, 0, 0), eTextAlign::ALIGN_LEFT);

        //ID
        Draw::DrawGxtText(27, m_Ped->hPed, 0, CVector2D(491, 155), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
        
        //Nome
        Draw::DrawGxtText(m_Ped->name.gxtId, 0, 0, CVector2D(365, 241), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Data de nascimento
        Draw::DrawGxtText(42, m_Ped->birthDay, m_Ped->birthMonth, CVector2D(365, 288), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
        Draw::DrawGxtText(43, m_Ped->birthYear, 0, CVector2D(420, 288), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //CPF
        Draw::DrawGxtText(41, m_Ped->cpf[0], m_Ped->cpf[1], CVector2D(365, 336), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
    } else {
        //Fundo CNH
        Draw::DrawSprite(102, CVector2D(200, 120), CVector2D(430, 275), CRGBA(150, 150, 150));

        //TEST text
        Draw::DrawGxtText(2, 999, 999, m_TestPosition, CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Nome
        Draw::DrawGxtText(m_Ped->name.gxtId, 0, 0, CVector2D(276, 200), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //ID
        Draw::DrawGxtText(1, m_Ped->hPed, 0, CVector2D(412, 247), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Nº de Registro
        Draw::DrawGxtText(1, m_Ped->cnhRegisterNum, 0, CVector2D(279, 367), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);

        //Validade
        Draw::DrawGxtText(42, m_Ped->cnhValidDay, m_Ped->cnhValidMonth, CVector2D(426, 367), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
        Draw::DrawGxtText(43, m_Ped->cnhValidYear, 0, CVector2D(480, 367), CRGBA(0, 0, 0), eTextAlign::ALIGN_LEFT);
    }
}