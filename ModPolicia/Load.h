#pragma once

#include "pch.h"

#include <functional>

class Load {
public:
    static void AddModelToLoad(int modelId);
    static void AddAnimationToLoad(std::string anim);

    static void LoadAll(std::function<void()> callback);

private:
    static void LoadFirstModel();
    static void ModelLoaded(int modelId);
};