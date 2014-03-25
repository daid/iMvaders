#include <string.h>
#include "scoreManager.h"

ScoreManager score;

ScoreManager::ScoreManager()
{
    currentScore = 0;
    displayScore = 0;
    highscoreList[0][0].score = 100;
    highscoreList[0][0].name = "DAV";
    highscoreList[0][1].score = 20;
    highscoreList[0][1].name = "JME";
    highscoreList[0][2].score = 10;
    highscoreList[0][2].name = "MRV";

    highscoreList[1][0].score = 100;
    highscoreList[1][0].name = "DAV JME";
    highscoreList[1][1].score = 20;
    highscoreList[1][1].name = "JME MRV";
    highscoreList[1][2].score = 10;
    highscoreList[1][2].name = "MRV MRV";
    
    loadHighscore("score.list");
}

void ScoreManager::add(int points)
{
    currentScore += points;
}

int ScoreManager::get()
{
    //TODO: Add proper timed animation for score increase.
    if (displayScore < currentScore)
        displayScore++;
    else
        displayScore = currentScore;
    return displayScore;
}

void ScoreManager::reset()
{
    currentScore = 0;
}

bool ScoreManager::isHighscore(int playerCount)
{
    assert(playerCount > 0 && playerCount <= MAX_PLAYERS);
    if (currentScore > highscoreList[playerCount-1][highscoreListSize-1].score)
        return true;
    return false;
}

void ScoreManager::enterHighscore(int playerCount, std::string name)
{
    assert(playerCount > 0 && playerCount <= MAX_PLAYERS);
    if (name.size() < playerCount)
        return;
    int n=highscoreListSize-1;
    while(n>0 && highscoreList[playerCount-1][n-1].score < currentScore)
        n--;
    for(int i=highscoreListSize-1; i>n; i--)
        highscoreList[playerCount-1][i] = highscoreList[playerCount-1][i-1];
    highscoreList[playerCount-1][n].score = currentScore;
    highscoreList[playerCount-1][n].name = name;
    
    saveHighscore("score.list");
}

int ScoreManager::getHighScore(int playerCount, int idx)
{
    assert(idx >= 0 && idx < highscoreListSize);
    assert(playerCount > 0 && playerCount <= MAX_PLAYERS);
    return highscoreList[playerCount-1][idx].score;
}
std::string ScoreManager::getHighscoreName(int playerCount, int idx)
{
    assert(idx >= 0 && idx < highscoreListSize);
    assert(playerCount > 0 && playerCount <= MAX_PLAYERS);
    return highscoreList[playerCount-1][idx].name;
}

void ScoreManager::update(float delta)
{
    
}

void ScoreManager::loadHighscore(std::string filename)
{
    FILE* f = fopen(filename.c_str(), "r");
    if (!f)
        return;
    for(int p=0; p<MAX_PLAYERS; p++)
    {
        for(int i=0; i<highscoreListSize; i++)
        {
            char buffer[64];
            if (fgets(buffer, sizeof(buffer), f))
            {
                while(strlen(buffer) > 0 && buffer[strlen(buffer)-1] <= ' ')
                    buffer[strlen(buffer)-1] = '\0';
                highscoreList[p][i].name = buffer;
                fgets(buffer, sizeof(buffer), f);
                highscoreList[p][i].score = atoi(buffer);
            }
        }
    }
    fclose(f);
}

void ScoreManager::saveHighscore(std::string filename)
{
    FILE* f = fopen(filename.c_str(), "w");
    if (!f)
        return;
    for(int p=0; p<MAX_PLAYERS; p++)
    {
        for(int i=0; i<highscoreListSize; i++)
        {
            fprintf(f, "%s\n%d\n", highscoreList[p][i].name.c_str(), highscoreList[p][i].score);
        }
    }
    fclose(f);
}
