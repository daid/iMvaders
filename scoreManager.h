#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

/**
    The score manager manages the score and high-score for the game. You can add points with
        score.add(10);
    You can get the current score with:
        score.get();
*/

class ScoreManager;
extern ScoreManager score;
class ScoreManager
{
private:
    int displayScore;
    int currentScore;
public:
    ScoreManager();
    
    void add(int points);
    int get();
};

#endif//SCORE_MANAGER_H
