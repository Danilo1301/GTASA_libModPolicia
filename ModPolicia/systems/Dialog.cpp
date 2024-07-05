#include "Dialog.h"

std::map<eDialogId, Dialog> Dialogs::m_Dialogs = {
    { eDialogId::DIALOG_COMING_FROM, { 175, { 176, 177, 178, 179 } } },
    { eDialogId::DIALOG_VEHICLE_OWNER, { 180, { 181, 182, 183 } } },
    { eDialogId::DIALOG_CRIMES, { 185, { 186, 187, 188 } } },
    { eDialogId::DIALOG_ARRESTED, { 189, { 190, 191 } } },
    { eDialogId::DIALOG_ILLEGAL_STUFF_IN_CAR, { 218, { 219, 220 } } }
};