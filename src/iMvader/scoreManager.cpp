#include "scoreManager.h"

ScoreManager score;

ScoreManager::ScoreManager()
{
    currentScore = 0;
    displayScore = 0;
    highscoreList[0].score = 9003;
    highscoreList[0].name = "DAV";
    highscoreList[1].score = 9002;
    highscoreList[1].name = "JME";
    highscoreList[2].score = 9001;
    highscoreList[2].name = "MRV";
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

int ScoreManager::getHighScore(int idx)
{
    if (idx < 0 || idx >= highscoreListSize)
        return 0;
    return highscoreList[idx].score;
}
std::string ScoreManager::getHighscoreName(int idx)
{
    if (idx < 0 || idx >= highscoreListSize)
        return "";
    return highscoreList[idx].name;
}
