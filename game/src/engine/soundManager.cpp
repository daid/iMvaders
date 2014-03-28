#include <stdio.h>
#include <string.h>
#include <SFML/Network.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

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

std::string url_encode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        }
        else if (c == ' ')  {
            escaped << '+';
        }
        else {
            escaped << '%' << std::setw(2) << ((int) c) << std::setw(0);
        }
    }

    return escaped.str();
}

void SoundManager::playTextToSpeech(std::string text)
{
    std::string name = "TTS:" + text;
    sf::SoundBuffer* data = soundMap[name];
    if (data != NULL)
    {
        playSoundData(data, 1.0, 100.0);
        return;
    }

    sf::Http http("localhost", 59125);
    sf::Http::Request request("process?INPUT_TEXT=" + url_encode(text) + "&INPUT_TYPE=TEXT&OUTPUT_TYPE=AUDIO&AUDIO=WAVE_FILE&LOCALE=en_US&VOICE=dfki-prudence");
    sf::Http::Response response = http.sendRequest(request);
    
    sf::Http::Response::Status status = response.getStatus();
    if (status == sf::Http::Response::Ok)
    {
        std::string wave = response.getBody();
        sf::SoundBuffer* data = new sf::SoundBuffer();
        data->loadFromMemory(wave.data(), wave.size());
        soundMap[name] = data;
        playSoundData(data, 1.0, 100.0);
    }
    else
    {
        std::cout << "Error requesting text to speech from Mary server: " << status << std::endl;
    }
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
