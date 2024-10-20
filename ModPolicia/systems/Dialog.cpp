#include "Dialog.h"

std::map<eDialogId, Dialog> Dialogs::m_Dialogs = {
    {
        eDialogId::DIALOG_COMING_FROM,
        {
            "De onde voce ta vindo?",
            175,
            {
                176,
                177,
                178,
                179
            }
        }
    },
    {
        eDialogId::DIALOG_VEHICLE_OWNER,
        {
            "De quem e o veiculo?",
            180,
            {
                181,
                182,
                183
            }
        }
    },
    {
        eDialogId::DIALOG_CRIMES,
        {
            "Tem passagem?",
            185,
            {
                186,
                187,
                188
            }
        }
    },
    {
        eDialogId::DIALOG_ARRESTED,
        {
            "Ja foi preso?",
            189,
            {
                190,
                191
            }
        }
    },
    {
        eDialogId::DIALOG_ILLEGAL_STUFF_IN_CAR,
        {
            "Ta com algo de errado no carro?",
            218,
            {
                219,
                220
            }
        }
    }
};