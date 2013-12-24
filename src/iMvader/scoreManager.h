#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#include <SFML/System.hpp>
#include "Updatable.h"
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
    } highscoreList[highscoreListSize];
public:
    ScoreManager();
    
    void add(int points);
    int get();
    void reset();
    
    int getHighScore(int idx);
    std::string getHighscoreName(int idx);
    
    virtual void update(float delta);
};

#endif//SCORE_MANAGER_H
