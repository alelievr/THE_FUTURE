#pragma once
#include "shaderpixel.h"
#include "SFML/Audio.hpp"
#include <vector>

class AudioManager
{
	private:
		std::vector< sf::SoundBuffer >	_audioBuffers;
		sf::Sound						_sound;
		int								_currentPlayingIndex;
		int								_audioTexture;

	public:
		AudioManager(void);
		AudioManager(const AudioManager & rhs) = delete;

		AudioManager &	operator=(const AudioManager & rhs) = delete;

		bool	LoadAudioFile(const std::string & fileName);
		bool	Play(const size_t index);
		bool	Volume(const float vol); //vol [0-1]
		bool	Pause(void);
		int		GetCurrentAudioTexture(void);
		
		virtual ~AudioManager(void);
};
