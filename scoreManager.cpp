#include "scoreManager.h"

ScoreManager score;

ScoreManager::ScoreManager()
{
    currentScore = 0;
    displayScore = 0;
    highscoreList[0].score = 3000;
    highscoreList[0].name = "DAV";
    highscoreList[1].score = 2000;
    highscoreList[1].name = "JMY";
    highscoreList[2].score = 1000;
    highscoreList[2].name = "BRT";
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

int ScoreManager::getHighScore(int idx)
{
    if (idx < 0 || idx >= highscoreListSize)
        return 0;
    return highscoreList[idx].score;
}
sf::String ScoreManager::getHighscoreName(int idx)
{
    if (idx < 0 || idx >= highscoreListSize)
        return "";
    return highscoreList[idx].name;
}
