#include <stdio.h>
#include <string.h>

#include "soundManager.h"

#define MAX_SOUNDS 16

SoundManager soundManager;

SoundManager::SoundManager()
{
    //By creating a SoundBuffer we force SFML to load the sound subsystem. Else this is done when the first sound is loaded, causing a delay.
    sf::SoundBuffer forceLoadBuffer;

    for(unsigned int n=0; n<MAX_SOUNDS; n++)
        activeSoundList.push_back(sf::Sound());
}

SoundManager::~SoundManager()
{
}

void SoundManager::playSound(std::string name, float pitch, float volume)
{
    sf::SoundBuffer* data = soundMap[name];
    if (data == NULL)
        data = loadSound(name);
    
    playSoundData(data, pitch, volume);
}

void SoundManager::setTextToSpeachVoice(std::string name)
{
}

void SoundManager::playTextToSpeech(std::string text)
{
}

void SoundManager::playSoundData(sf::SoundBuffer* data, float pitch, float volume)
{
    for(unsigned int n=0; n<activeSoundList.size(); n++)
    {
        sf::Sound& sound = activeSoundList[n];
        if (sound.getStatus() == sf::Sound::Stopped)
        {
            sound.setBuffer(*data);
            sound.setPitch(pitch);
            sound.setVolume(volume);
            sound.play();
            return;
        }
    }
}

sf::SoundBuffer* SoundManager::loadSound(std::string name)
{
    sf::SoundBuffer* data = soundMap[name];
    if (data)
        return data;
    
    data = new sf::SoundBuffer();
    char buffer[128];
    sprintf(buffer, "resources/%s.wav", name.c_str());
    if (!data->loadFromFile(buffer))
    {
        printf("Failed to load: %s\n", buffer);
        return NULL;
    }
    
    printf("Loaded: %s of %f seconds\n", buffer, data->getDuration().asSeconds());
    soundMap[name] = data;
    return data;
}
