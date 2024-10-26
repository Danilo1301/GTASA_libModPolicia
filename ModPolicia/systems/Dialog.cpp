#include "Dialog.h"

std::map<eDialogId, Dialog> Dialogs::m_Dialogs = {
    {
        eDialogId::DIALOG_COMING_FROM,
        {
            "DIALOG_COMING_FROM",
            {
                "DIALOG_COMING_FROM my_house",
                "DIALOG_COMING_FROM family_house",
                "DIALOG_COMING_FROM friend_house",
                "DIALOG_COMING_FROM work"
            }
        }
    },
    {
        eDialogId::DIALOG_VEHICLE_OWNER,
        {
            "DIALOG_VEHICLE_OWNER",
            {
                "DIALOG_VEHICLE_OWNER mine",
                "DIALOG_VEHICLE_OWNER dad",
                "DIALOG_VEHICLE_OWNER unknown"
            }
        }
    },
    {
        eDialogId::DIALOG_CRIMES,
        {
            "DIALOG_CRIMES",
            {
                "DIALOG_CRIMES no",
                "DIALOG_CRIMES yes",
            }
        }
    },
    {
        eDialogId::DIALOG_ARRESTED,
        {
            "DIALOG_ARRESTED",
            {
                "DIALOG_ARRESTED no",
                "DIALOG_ARRESTED yes"
            }
        }
    },
    {
        eDialogId::DIALOG_ILLEGAL_STUFF_IN_CAR,
        {
            "DIALOG_ILLEGAL_STUFF_IN_CAR",
            {
                "DIALOG_ILLEGAL_STUFF_IN_CAR no",
                "DIALOG_ILLEGAL_STUFF_IN_CAR yes"
            }
        }
    }
};