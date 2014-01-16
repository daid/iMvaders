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

void SoundManager::playSound(const char* name, float pitch, float volume)
{
    for(unsigned int n=0; n<activeSoundList.size(); n++)
    {
        sf::Sound& sound = activeSoundList[n];
        if (sound.getStatus() == sf::Sound::Stopped)
        {
            sf::SoundBuffer* data = soundMap[name];
            if (data == NULL)
                data = loadSound(name);
            
            sound.setBuffer(*data);
            sound.setPitch(pitch);
            sound.setVolume(volume);
            sound.play();
            return;
        }
    }
    if (activeSoundList.size() >= 8)
        return;
    activeSoundList.push_back(sf::Sound());
    sf::Sound& sound = activeSoundList[activeSoundList.size() - 1];
    sf::SoundBuffer* data = soundMap[name];
    if (data == NULL)
        data = loadSound(name);

    sound.setBuffer(*data);
    sound.setPitch(pitch);
    sound.setVolume(volume);
    sound.play();
}

sf::SoundBuffer* SoundManager::loadSound(const char* name)
{
    sf::SoundBuffer* data = soundMap[name];
    if (data)
        return data;
    
    data = new sf::SoundBuffer();
    char buffer[128];
    sprintf(buffer, "resources/%s.wav", name);
    if (!data->loadFromFile(buffer))
    {
        printf("Failed to load: %s\n", buffer);
        return NULL;
    }
    
    printf("Loaded: %s of %f seconds\n", buffer, data->getDuration().asSeconds());
    soundMap[name] = data;
    return data;
}
