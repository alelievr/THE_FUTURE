#include "AudioManager.hpp"

AudioManager::AudioManager(void) : _currentPlayingIndex(-1)
{
	_audioTexture = -1;//!TODO
}

AudioManager::~AudioManager(void) {}

bool		AudioManager::LoadAudioFile(const std::string & fileName)
{
	sf::SoundBuffer	buffer;

	std::cout << "loading audio file: " << fileName << std::endl;
	if (!buffer.loadFromFile(fileName))
		return false;
	_audioBuffers.push_back(buffer);
	return true;
}

bool		AudioManager::Play(const size_t index)
{
	if (index >= _audioBuffers.size())
		return false;

	if (static_cast< size_t >(_currentPlayingIndex) == index)
		return true;

	if (_currentPlayingIndex != -1 && _sound.getStatus() != sf::Music::Status::Stopped)
		_sound.stop();
	_sound.setBuffer(_audioBuffers[index]);
	std::cout << "playing " << index << std::endl;
	_sound.play();
	_sound.setLoop(true);
	_currentPlayingIndex = index;
	return true;
}

bool		AudioManager::Volume(const float vol)
{
	int		volume = (int)(vol * 100);
	std::cout << "volume " << vol << std::endl;
	_sound.setVolume(volume);
	return true;
}

bool		AudioManager::Pause(void)
{
	std::cout << "pause" << std::endl;
	if (_sound.getStatus() == sf::Music::Status::Playing)
		_sound.pause();
	else
		return false;
	return true;
}

int			AudioManager::GetCurrentAudioTexture(void)
{
	if (_currentPlayingIndex == -1)
		return -1;

	const auto &		b = _audioBuffers[_currentPlayingIndex];
	//const sf::Int16 *	samples = b.getSamples();
	size_t			sampleRate = b.getSampleRate();
	size_t			channelCount = b.getChannelCount();
	sf::Time		duration = b.getDuration();
	sf::Time		offset = _sound.getPlayingOffset();

	std::cout << "playing offset: " << offset.asMilliseconds() << ", sampler rate: " << sampleRate << ", channelCount: " << channelCount << ", duration: " << duration.asMilliseconds() << std::endl;

	return _audioTexture;
}
