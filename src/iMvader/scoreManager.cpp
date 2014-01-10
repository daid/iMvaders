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
    int n=highscoreListSize-1;
    while(n>0 && highscoreList[playerCount-1][n-1].score < currentScore)
        n--;
    for(int i=highscoreListSize-1; i>n; i--)
        highscoreList[playerCount-1][i] = highscoreList[playerCount-1][i-1];
    highscoreList[playerCount-1][n].score = currentScore;
    highscoreList[playerCount-1][n].name = name;
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
