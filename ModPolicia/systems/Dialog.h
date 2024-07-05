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
    int gxtId;
    std::vector<int> responses;
};

class Dialogs {
public:
    static std::map<eDialogId, Dialog> m_Dialogs;
};