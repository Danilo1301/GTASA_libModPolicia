#pragma once

#include "../menu/Menu.h"

struct RadioFrequency
{
	int textureId;
};

struct RadioChannel {
	int textureId;
	std::vector<RadioFrequency> frequencies;
};

class WindowRadio {
public:
	static Window* m_Window;
	static bool m_Enabled;
	static CVector2D m_Position;

	static std::vector<RadioChannel> m_Channels;
	static int m_CurrentChannel;
	static int m_CurrentFrequency;
	static bool m_ChangingChannels;

	static void Create();
	static void Remove();

	static void CreateTestOptions();

	static void ToggleRadio(bool enabled);
	static void ToggleRadioOff(bool keepAnimation);
	static void Update(int dt);
	static void Draw();

	static void SelectFrequency(int channelId, int frequencyId);
};