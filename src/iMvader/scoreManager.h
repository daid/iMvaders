#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#include <SFML/System.hpp>
#include "Updatable.h"
#include "Player.h"
/**
    The score manager manages the score and high-score for the game. You can add points with
        score.add(10);
    You can get the current score with:
        score.get();
*/

class ScoreManager : public Updatable
{
public:
    static const int highscoreListSize = 3;
private:
    int displayScore;
    int currentScore;
    
    struct {
        int score;
        std::string name;
    } highscoreList[MAX_PLAYERS][highscoreListSize];
public:
    ScoreManager();
    
    void add(int points);
    int get();
    void reset();
    
    bool isHighscore(int playerCount);
    void enterHighscore(int playerCount, std::string name);
    int getHighScore(int playerCount, int idx);
    std::string getHighscoreName(int playerCount, int idx);
    
    virtual void update(float delta);
};

#endif//SCORE_MANAGER_H
