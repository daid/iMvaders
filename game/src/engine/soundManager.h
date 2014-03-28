#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SFML/Audio.hpp>

#include <map>
#include <vector>

class SoundManager;
extern SoundManager soundManager;
class SoundManager
{
private:
    std::map<std::string, sf::SoundBuffer*> soundMap;
    std::vector<sf::Sound> activeSoundList;
public:
    SoundManager();
    ~SoundManager();
    
    void playSound(std::string name, float pitch = 1.0f, float volume = 100.0f);
    void setTextToSpeachVoice(std::string name);
    void playTextToSpeech(std::string text);
private:
    void playSoundData(sf::SoundBuffer* data, float pitch, float volume);
    sf::SoundBuffer* loadSound(std::string name);
};

#endif//SOUNDMANAGER_H
