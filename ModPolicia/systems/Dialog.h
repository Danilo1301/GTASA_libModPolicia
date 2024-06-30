#pragma once

#include "pch.h"

enum eDialogId {
    DIALOG_COMING_FROM,
    DIALOG_VEHICLE_OWNER,
    DIALOG_CRIMES
};

struct Dialog {
    int gxtId;
    std::vector<int> responses;
};

class Dialogs {
public:
    static std::map<eDialogId, Dialog> m_Dialogs;
};