#include "scoreManager.h"

ScoreManager score;

ScoreManager::ScoreManager()
{
    currentScore = 0;
    displayScore = 0;
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
