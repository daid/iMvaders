#include <stdio.h>

#include "soundManager.h"

SoundManager soundManager;

SoundManager::SoundManager()
{
    //By creating a SoundBuffer we force SFML to load the sound subsystem. Else this is done when the first sound is loaded, causing a delay.
    sf::SoundBuffer forceLoadBuffer;
}

SoundManager::~SoundManager()
{
}

void SoundManager::setSound(sf::Sound& sound, const char* name)
{
    sf::SoundBuffer& data = soundMap[name];
    if (data.getSampleCount() < 1)
        loadSound(name);
    
    sound.setBuffer(data);
}

void SoundManager::loadSound(const char* name)
{
    sf::SoundBuffer& data = soundMap[name];
    
    char buffer[128];
    sprintf(buffer, "resources/%s.wav", name);
    if (!data.loadFromFile(buffer))
    {
        printf("Failed to load: %s\n", buffer);
        return;
    }
    
    printf("Loaded: %s of %f seconds\n", buffer, data.getDuration().asSeconds());
}

