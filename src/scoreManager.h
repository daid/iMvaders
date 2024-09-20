#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#include "Updatable.h"
#include "player.h"
#include "stringImproved.h"
/**
    The score manager manages the score and high-score for the game. You can add points with
        score.add(10);
    You can get the current score with:
        score.get();
*/

class ScoreManager : public Updatable
{
public:
    static const int highscoreListSize = 6;
private:
    int displayScore;
    int currentScore;
    
    struct {
        int score;
        string name;
    } highscoreList[MAX_PLAYERS][highscoreListSize];
public:
    ScoreManager();
    virtual ~ScoreManager() {}
    
    void add(int points);
    int get();
    void reset();
    
    bool isHighscore(int playerCount);
    void enterHighscore(int playerCount, string name);
    int getHighScore(int playerCount, int idx);
    string getHighscoreName(int playerCount, int idx);
    
    virtual void update(float delta) override;

private:
    void loadHighscore(string filename);
    void saveHighscore(string filename);
};

#endif//SCORE_MANAGER_H
