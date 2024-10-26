#include "WindowDocument.h"

#include "Log.h"
#include "CleoFunctions.h"
#include "Pullover.h"
#include "SoundSystem.h"
#include "ModConfig.h"

extern IMenuVSL* menuVSL;

IWindow* WindowDocument::m_Window = NULL;
DOC_TYPE WindowDocument::m_DocumentType = DOC_TYPE::RG;
Ped* WindowDocument::m_Ped = NULL;
std::function<void()> WindowDocument::m_OnClose;

CVector2D WindowDocument::m_TestPosition = CVector2D(548, 404);

CSprite2d WindowDocument::m_RG_sprite;
CSprite2d WindowDocument::m_CNH_sprite;

void WindowDocument::Create()
{
    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "";
    window->m_Position = CVector2D(0, 0);

    auto editPos = window->AddButton("Edit position");
    editPos->onClick = [window]() {
        auto poswindow = menuVSL->AddVector2Window(window, &m_TestPosition, 0.0f, 10000.0f, 0.5f);
        poswindow->m_Position = CVector2D(1000, 500);
    };
    
    auto confirm_button = window->AddButton(GetLanguageLine("confirm"), CRGBA(0, 155, 255));
    confirm_button->m_FitInWindow = false;
    confirm_button->m_Position = CVector2D(1100, 350);
    confirm_button->m_Size = CVector2D(200, 50);
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
        auto confirm_consultarrg = window->AddButton(GetLanguageLine("check_id"), CRGBA(0, 155, 255));
        confirm_consultarrg->m_FitInWindow = false;
        confirm_consultarrg->m_Position = CVector2D(1100, 420);
        confirm_consultarrg->m_Size = CVector2D(200, 50);
        confirm_consultarrg->onClick = []()
        {
            Remove();

            SoundSystem::PlayHTAudio();
            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/CHECK_ID_", false);

            menuVSL->ShowMessage(GetLanguageLine("voice_check_id"), 3000);

            CleoFunctions::WAIT(4000, []() {
                auto ped = m_Ped;
                if(ped->isWanted)
                {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/ID_WITH_ARREST_WARRANT_", false);

                    menuVSL->ShowMessage(GetLanguageLine("voice_id_warrant"), 3000);
                } else {
                    SoundSystem::PlayHTAudio();
                    SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/ID_OK_", false);

                    menuVSL->ShowMessage(GetLanguageLine("voice_id_ok"), 3000);
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
    m_Window->SetToBeRemoved();
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

    auto ped = m_Ped;

    if(!m_RG_sprite.m_pTexture)
    {
        char path[512];
        sprintf(path, "%s/assets/rg.png", ModConfig::GetConfigFolder().c_str());
        m_RG_sprite.m_pTexture = (RwTexture*)menuVSL->LoadRwTextureFromFile(path, "rg", true);
    }

    if(!m_CNH_sprite.m_pTexture)
    {
        char path[512];
        sprintf(path, "%s/assets/cnh.png", ModConfig::GetConfigFolder().c_str());
        m_CNH_sprite.m_pTexture = (RwTexture*)menuVSL->LoadRwTextureFromFile(path, "cnh", true);
    }

    CRGBA textColor = CRGBA(230, 230, 230);

    if(m_DocumentType == DOC_TYPE::RG)
    {
        // Fundo RG
        menuVSL->DrawSprite(&m_RG_sprite, CVector2D(180, 180), CVector2D(816, 522));

        // RG
        menuVSL->DrawString(ped->rg, CVector2D(345, 228), textColor, eFontAlignment::ALIGN_LEFT);

        // ID
        menuVSL->DrawString(std::to_string(ped->hPed), CVector2D(731, 232), textColor, eFontAlignment::ALIGN_LEFT);

        // Nome
        menuVSL->DrawString(ped->name.name, CVector2D(488, 395), textColor, eFontAlignment::ALIGN_LEFT);
        
        // Data de nascimento
        
        std::string date = FormatDate(ped->birthDay, ped->birthMonth, ped->birthYear);
        menuVSL->DrawString(date, CVector2D(488, 483), textColor, eFontAlignment::ALIGN_LEFT);

        // CPF
        menuVSL->DrawString(ped->cpf, CVector2D(491, 574), textColor, eFontAlignment::ALIGN_LEFT);
    }
    if(m_DocumentType == DOC_TYPE::CNH)
    {
        // Fundo CNH
        menuVSL->DrawSprite(&m_CNH_sprite, CVector2D(180, 180), CVector2D(816, 522));

        // Nome
        menuVSL->DrawString(ped->name.name, CVector2D(322, 318), textColor, eFontAlignment::ALIGN_LEFT);

        // ID
        menuVSL->DrawString(std::to_string(ped->hPed), CVector2D(580, 405), textColor, eFontAlignment::ALIGN_LEFT);

        // Nº Registro
        menuVSL->DrawString(std::to_string(ped->cnhRegisterNum), CVector2D(327, 633), textColor, eFontAlignment::ALIGN_LEFT);

        // Validade
        std::string validDate = FormatDate(ped->cnhValidDay, ped->cnhValidMonth, ped->cnhValidYear);
        menuVSL->DrawString(validDate, CVector2D(604, 633), textColor, eFontAlignment::ALIGN_LEFT);
    }

    //menuVSL->DrawString("This is a ~r~string", m_TestPosition, CRGBA(0, 0, 0), eFontAlignment::ALIGN_LEFT);
}