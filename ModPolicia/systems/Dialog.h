#pragma once

#include "pch.h"

enum eDialogId {
    DIALOG_COMING_FROM,
    DIALOG_VEHICLE_OWNER,
    DIALOG_CRIMES,
    DIALOG_ARRESTED,
    DIALOG_ILLEGAL_STUFF_IN_CAR
};

struct Dialog {
    std::string question;
    std::vector<std::string> responses;
};

class Dialogs {
public:
    static std::map<eDialogId, Dialog> m_Dialogs;
};