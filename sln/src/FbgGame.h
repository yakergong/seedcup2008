#ifndef FBG_GAME_H
#define FBG_GAME_H

#include "FbgBlock.h"
#include "TetrisAI.h"
#include "Logger.h"
#include "BlockFactory.h"
#include "AIModule.h"

#include <SDL/SDL.h>
#include <vector>
#include <string>
#include <physfs/physfs.h>
#include <utility>

// When line(s) have been gained
class   FbgLineGain
{
public:
    unsigned char lineCount;            // 1-4 lines removed
    short   linePos[4];
    short   lineMatrix[4][10];
    int     levelGained;                // Level that was attained
    FbgLineGain()
    {
        levelGained = -1;
        lineCount = 0;
        linePos[0] = -1;
        linePos[1] = -1;
        linePos[2] = -1;
        linePos[3] = -1;
    }
};

enum FbgGameEvent
{
    FBG_EVENT_DROP = 1,
    FBG_EVENT_UNKNOWN,
};

enum FbgGameState
{
    GAMEPLAY = 0,
    GAMEOVER,
    QUIT,
};

const bool fbgGameStatePauseMask[] = { false, true, false };

class FbgGame
{
public:
    // Constructors
    FbgGame(int id, bool idleMode, int startLevel, std::string mode);
    // Destructor
    ~FbgGame();

    // Event Functions
    static inline void issue(const int &eventCode, FbgGame *game);
    void processEvent(const SDL_Event & event);
    // void gameplayLoop();

    // Block Management
    void finishCurBlock();

    short getMatrixAt(int row, int col)
    {
        return matrix_[row][col];
    }

    FbgBlock *getCurBlock()
    {
        return &curBlock;
    }

    // Accessors
    FbgLineGain *getLineGain()
    {
        return lineGain;
    }
    FbgBlock *getBlockSet(int index)
    {
        return &blockSet[index];
    }
    short   getNextBlockIndex()
    {
        return nextBlockIndex;
    }

    unsigned char getLevel()
    {
        return level_;
    }

    int getLines()
    {
        return lines_;
    }

    int getTotalLines();

    long getScore()
    {
        return score_;
    }

    long getTotalScore();

    bool getPaused()
    {
        return paused;
    }

    void setPaused(bool newP)
    {
        paused = newP;
    }

    FbgGameState getState()
    {
        return state_;
    }

    void setState(FbgGameState newState)
    {
        state_ = newState;
        setPaused(fbgGameStatePauseMask[newState]);
    }

    bool getIdleMode()
    {
        return isIdleMode_;
    }

    int getID() const
    {
        return id_;
    }

    std::string getMode() const
    {
        return mode_;
    }

    std::pair<float, float> getDestCoord() const
    {
        return destCoord_;
    }

    bool getDownHeld() const
    {
        return downHeld_;
    }

    void  startNewLevel();

    void setSpeedUpMode(bool speedUp)
    {
        speedUpMode_ = speedUp;
    }
private:
    int id_;
    std::string mode_;
    Logger log_;
    Logger scoreLog_;
    BlockFactory blockFactory_;
    AIModule aiModule_;
    int  level_;                     // Current level
    bool levelEndMark_;
    bool speedUpMode_;               // 是否在加速模式下运行(快速获取结果)
    std::vector< int > scorePerLevel_;
    std::vector< int > linesPerLevel_;
    std::vector< std::vector< MoveState > > moveStates_;
    // 多窗口模式下的目标位置
    std::pair<float, float> destCoord_;
    int newLevelPause_;

    // Board-related
    short   matrix_[18][10];                // Game board
    std::vector < FbgBlock > blockSet;     // Different pieces
    FbgBlock curBlock;                     // Needed in case we rotate/nudge
    short   nextBlockIndex;                // Next block's type
    FbgLineGain *lineGain;                 // If we're currently animating a line gain this will != NULL
    FbgGameState state_;                    // The current game state
    bool  paused;                        // If gameplay is paused (ie during a line gain)
    bool  isIdleMode_;                   // 闲置模式,闲置模式下无消息则等待，非闲置模式下程序强行循环
    bool  downHeld_;
    // Game-related
    int     lines_;                         // Number of lines removed
    int     score_;                        // Current score

    // 保存操作的录像
    void SaveLogFile();
    // 保存分数
    void SaveScoreLogFile();

    // Block Management
    void cycleBlocks();

    void putBlockInMatrix(const FbgBlock & theBlock);
    void setupBlocks();

    bool getSpeedUpMode()
    {
        return speedUpMode_;
    }

    void AIMove(MoveState move);
};

#endif
