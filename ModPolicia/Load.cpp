#include "Load.h"

#include "Log.h"
#include "CleoFunctions.h"

std::vector<int> modelsToLoad;
std::function<void()> onLoadAllCallback;

void Load::AddModelToLoad(int modelId)
{
    modelsToLoad.push_back(modelId);
}

void Load::AddAnimationToLoad(std::string anim)
{
    // TODO
}

void Load::LoadAll(std::function<void()> callback)
{
    onLoadAllCallback = callback;

    if(modelsToLoad.size() == 0) return;

    LoadFirstModel();
}

void Load::LoadFirstModel()
{
    auto modelId = modelsToLoad[0];

    if(CleoFunctions::MODEL_AVAILABLE(modelId))
    {
        ModelLoaded(modelId);
        return;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load: Loading " << modelId << " (" << modelsToLoad.size() << " left)" << std::endl;

    CleoFunctions::LOAD_MODEL(modelId);
    CleoFunctions::LOAD_REQUESTED_MODELS();

    if(CleoFunctions::MODEL_AVAILABLE(modelId))
    {
        ModelLoaded(modelId);
        return;
    }

    CleoFunctions::AddWaitForFunction([modelId]() {
        return CleoFunctions::MODEL_AVAILABLE(modelId);
    }, [modelId]() {
        ModelLoaded(modelId);
    });
}

void Load::ModelLoaded(int modelId)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load: Model " << modelId << " loaded!" << std::endl;

    auto it = std::find(modelsToLoad.begin(), modelsToLoad.end(), modelId);
    modelsToLoad.erase(it);

    if(modelsToLoad.size() == 0)
    {
        onLoadAllCallback();
        return;
    }

    LoadFirstModel();
}