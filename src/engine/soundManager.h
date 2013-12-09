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
    std::map<sf::String, sf::SoundBuffer> soundMap;
public:
    SoundManager();
    ~SoundManager();
    
    void setSound(sf::Sound& sound, const char* name);
private:
    void loadSound(const char* name);
};

#endif//SOUNDMANAGER_H
