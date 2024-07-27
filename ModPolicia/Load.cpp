#include "Load.h"

#include "Log.h"
#include "CleoFunctions.h"

std::vector<int> modelsToLoad;
std::function<void()> onLoadAllCallback;

void Load::AddModelToLoad(int modelId)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load: Adding model " << modelId << std::endl;

    modelsToLoad.push_back(modelId);
}

void Load::AddAnimationToLoad(std::string anim)
{
    
}

void Load::LoadAll(std::function<void()> callback)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load: LoadAll" << std::endl;
    
    onLoadAllCallback = callback;

    if(modelsToLoad.size() == 0) return;

    LoadFirstModel();
}

void Load::LoadFirstModel()
{
    auto modelId = modelsToLoad[0];

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load: Loading " << modelId << " (" << modelsToLoad.size() << " left)" << std::endl;

    if(CleoFunctions::MODEL_AVAILABLE(modelId))
    {
        ModelLoaded(modelId);
        return;
    }

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