#include "SoundSystem.h"
#include "Log.h"
#include "ModConfig.h"
#include "Mod.h"

extern IBASS* BASS;

extern CCamera* camera;

BASS_3DVECTOR pos(0, 0, 0), vel(0, 0, 0), front(0, -1.0, 0), top(0, 0, 1.0);
BASS_3DVECTOR bass_tmp(0.0f, 0.0f, 0.0f), bass_tmp2(0.0f, 0.0f, 0.0f), bass_tmp3(0.0f, 0.0f, 0.0f);

std::set<AudioStream*> SoundSystem::streams;

bool SoundSystem::Init()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "BASS = " << (uintptr_t)BASS << std::endl;

	if (BASS->Set3DFactors(1.0f, 0.3f, 1.0f) && BASS->Set3DPosition(&pos, &vel, &front, &top))
	{
		Log::Level(LOG_LEVEL::LOG_BOTH) << "Initializing SoundSystem..." << std::endl;

        //initialized = true;
        BASS->Apply3D();
        return true;
    }
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Could not initialize SoundSys" << std::endl;
    return false;
}

void SoundSystem::Update()
{
    //Log::file << "AudioSystem Update" << std::endl;

    CMatrix* pMatrix = camera->GetMatSA();
    CVector* pVec = &pMatrix->pos;

    bass_tmp = { pVec->y, pVec->z, pVec->x };
    bass_tmp2 = { pMatrix->at.y, pMatrix->at.z, pMatrix->at.x };
    bass_tmp3 = { pMatrix->up.y, pMatrix->up.z, pMatrix->up.x };
    BASS->Set3DPosition(&bass_tmp, nullptr, pMatrix ? &bass_tmp2 : nullptr, pMatrix ? &bass_tmp3 : nullptr);
    //Log::file << "BASS 3d pos: " << pVec->y << ", " << pVec->z << ", " << pVec->x << std::endl;

    // process all streams
    std::for_each(streams.begin(), streams.end(), [](AudioStream* stream) { stream->Process(); });
    
    // apply above changes
    BASS->Apply3D();
}

AudioStream* SoundSystem::LoadStream(std::string src, bool in3d)
{
    AudioStream* result = in3d ? new AudioStream3D(src) : new AudioStream(src);

    streams.insert(result);

    return result;
}

AudioStream* SoundSystem::LoadStreamFromAudiosFolder(std::string src, bool in3d)
{
    std::string audiosPath = ModConfig::GetConfigFolder() + "/audios/";

    AudioStream* result = LoadStream(audiosPath + src, in3d);

    return result;
}

AudioStream* SoundSystem::PlayStream(std::string src, bool loop)
{
    if(!BASS)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "BASS is not loaded: can't play stream" << std::endl;
        return NULL;
    }
    Log::Level(LOG_LEVEL::LOG_BOTH) << "BASS = " << (uintptr_t)BASS << std::endl;

    AudioStream* audioStream = LoadStream(src, false);

    if(loop) audioStream->Loop(true);
    audioStream->Play();

    return audioStream;
}

AudioStream* SoundSystem::PlayStreamFromAudiosFolder(std::string src, bool loop)
{
    std::string audiosPath = ModConfig::GetConfigFolder() + "/audios/";

    return PlayStream(audiosPath + src, loop);
}

AudioStream* SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation(std::string src, bool loop)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "SoundSystem: PlayStreamWithRandomVariation '" << src << "'" << std::endl;

    std::string audiosPath = ModConfig::GetConfigFolder() + "/audios/";

    int i = 1;
    bool canLoop = true;
    do
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Trying to find audio variation: " << i << std::endl;

        auto audioFile = audiosPath + src + std::to_string(i) + ".wav";

        if(ModConfig::FileExists(audioFile))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << audioFile << " found" << std::endl;

            i++;
        } else {
            Log::Level(LOG_LEVEL::LOG_BOTH) << audioFile << " not found" << std::endl;
            canLoop = false;
        }
    } while (canLoop);
    
    int numVariations = i - 1;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Found " << numVariations << " variations" << std::endl;

    auto newSrc = audiosPath + src + std::to_string(Mod::GetRandomNumber(1, numVariations)) + ".wav";

    return PlayStream(newSrc, loop);
}

void SoundSystem::PlayHTAudio()
{
    PlayStreamFromAudiosFolder("ht.wav", false);
}