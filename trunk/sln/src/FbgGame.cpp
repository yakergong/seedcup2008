/*
 * Falling Block Game
 * Copyright (C) 1999-2002 Jared Krinke <http://derajdezine.vze.com/>
 * 
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 or (at your option) any later version
 * as published by the Free Software Foundation.
 * 
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this distribution; if not, write to:
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307  USA
 * 
 * Jared Krinke
 * 
 * Deraj DeZine
 * http://derajdezine.vze.com/
 */

#include "FbgGame.h"
#include "FbgMusic.h"
#include "ConfigMgr.h"
#include <stdlib.h>
#include <string>
#include <SDL/SDL.h>

using namespace std;

FbgGame::FbgGame(int id, bool idleMode , int startLevel, string mode)
{
    id_ = id;
    mode_ = mode;
    level_ = startLevel;
    isIdleMode_  = idleMode;

    ConfigMgr config;
    if(config.GetIntegerValue(mode_ + ".single_mode"))
    {
        destCoord_.first  = 0.0f;
        destCoord_.second = 0.0f;
    }
    else
    {
        int x = id%3;
        int y = id/3;
        destCoord_.first  = x * (256.0f);
        destCoord_.second = y * (-384.0f);
    }

    downHeld_ = false;

    if(config.GetIntegerValue(mode_ + ".speed_up_mode"))
    {
        speedUpMode_ = true;
    }
    else
    {
        speedUpMode_ = false;
    }

    if(config.GetStringValue(mode_ + ".input_type") != "keyboard")
        aiModule_.Init(mode_, getID());

    // Setup next and current blocks
    blockFactory_.Init(mode_);
    levelEndMark_ = false;

    newLevelPause_ = config.GetIntegerValue(mode + ".new_level_pause");

    setupBlocks();
    lineGain = NULL;
    startNewLevel();
}
void FbgGame::AIMove(MoveState move)
{
    moveStates_[level_].push_back(move);

    for (int i=0; i<move.rotate; i++)
    {
        if (!getPaused())
        {
            getCurBlock()->rotateBlockRight();
        }
    }

    if (move.move >0)
    {
        for (int i=0; i<move.move; i++)
        {
            if (!getPaused())
            {
                getCurBlock()->moveBlockLeft();
            }
        }
    }

    if (move.move<0)
    {
        for (int i=0; i>move.move; i--)
        {
            if (!getPaused())
            {
                getCurBlock()->moveBlockRight();
            }
        }
    }
}
void FbgGame::cycleBlocks()
{
    curBlock = blockSet[nextBlockIndex];
    if(blockFactory_.IsThisLevelEnd())
    {
        nextBlockIndex = 0;
        levelEndMark_  = true;
    }
    else
    {
        int curBlockIndex = nextBlockIndex;

        ConfigMgr config;

        // TODO:智能旋转
        if(config.GetStringValue(mode_ + ".input_type") != "keyboard")
        {
            int matrix[18][10];
            for(int i = 0; i < 18; ++i)
                for(int j = 0; j < 10; ++j)
                    if(matrix_[i][j])
                        matrix[i][j] = true;
                    else
                        matrix[i][j] = false;
            bool curBlock[4][4];
            bool nextBlock[4][4];
            blockSet[curBlockIndex].copyToMatrix(curBlock);
            blockSet[nextBlockIndex].copyToMatrix(nextBlock);
            MoveState state;
            state = aiModule_.GetMoveStrategy(curBlock, nextBlock, matrix);
            AIMove(state);
        }

        nextBlockIndex = blockFactory_.GetNextBlock();
    }
}

void FbgGame::setupBlocks()
{
    // FIXME ugly code
    // O
    {
        bool tmp[16] = { false, true, true, false,
            false, true, true, false,
            false, false, false, false,
            false, false, false, false
        };
        FbgBlock tmp2(this, 0, tmp);
        blockSet.push_back(tmp2);
    }
    // I
    {
        bool tmp[16] = { true, true, true, true,
            false, false, false, false,
            false, false, false, false,
            false, false, false, false
        };
        FbgBlock tmp2(this, 1, tmp);
        blockSet.push_back(tmp2);
    }
    // L
    {
        bool tmp[16] = { true, true, true, false,
            true, false, false, false,
            false, false, false, false,
            false, false, false, false
        };
        FbgBlock tmp2(this, 2, tmp);
        blockSet.push_back(tmp2);
    }
    // J
    {
        bool tmp[16] = { true, true, true, false,
            false, false, true, false,
            false, false, false, false,
            false, false, false, false
        };
        FbgBlock tmp2(this, 3, tmp);
        blockSet.push_back(tmp2);
    }
    // S
    {
        bool tmp[16] = { false, true, true, false,
            true, true, false, false,
            false, false, false, false,
            false, false, false, false
        };
        FbgBlock tmp2(this, 4, tmp);
        blockSet.push_back(tmp2);
    }
    // Z
    {
        bool tmp[16] = { true, true, false, false,
            false, true, true, false,
            false, false, false, false,
            false, false, false, false
        };
        FbgBlock tmp2(this, 5, tmp);
        blockSet.push_back(tmp2);
    }
    // T
    {
        bool tmp[16] = { true, true, true, false,
            false, true, false, false,
            false, false, false, false,
            false, false, false, false
        };
        FbgBlock tmp2(this, 6, tmp);
        blockSet.push_back(tmp2);
    }
}

void FbgGame::putBlockInMatrix(const FbgBlock & theBlock)
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (theBlock.getMatrixAt(row, col))
                matrix_[theBlock.getPosY() + row][theBlock.getPosX() + col] =
                    theBlock.getIndex() + 1;
        }
    }
}
void FbgGame::finishCurBlock()
{
    score_ += 10;

    // Cycle current, next blocks
    putBlockInMatrix(curBlock);
    cycleBlocks();

    // Check for full lines
    lineGain = new FbgLineGain();

    for (int row = 17; row >= 0; row--)
    {
        int col;
        for (col = 0; col < 10; col++)
        {
            if (matrix_[row][col] <= 0)
                break;
        }
        if (col == 10)
        {
            lineGain->lineCount++;
            lineGain->linePos[lineGain->lineCount - 1] = row;
            for (int c = 0; c < 10; c++)
                lineGain->lineMatrix[lineGain->lineCount - 1][c] = matrix_[row][c];
        }
    }

    // Remove lines from matrix
    int gap = 0;                        // Space between where we're copying from/to
    for (int row = 17; row >= 0; row--)
    {
        if (gap > 0)
        {
            for (int col = 0; col < 10; col++)
            {
                if (row + gap <= 17)
                    matrix_[row + gap][col] = matrix_[row][col];
            }
        }
        if (row == lineGain->linePos[0] || row == lineGain->linePos[1]
            || row == lineGain->linePos[2] || row == lineGain->linePos[3])
            gap++;
    }
    // Clear the lines we never got to due to the gap
    for (int row = gap - 1; row >= 0; row--)
    {
        for (int col = 0; col < 10; col++)
            matrix_[row][col] = 0;
    }

    // Adjust Score, Line count
    lines_ += lineGain->lineCount;
    if (lineGain->lineCount == 1)
        score_ += 100/*40 * level*/;
    else if (lineGain->lineCount == 2)
        score_ += 300/*100 * level*/;
    else if (lineGain->lineCount == 3)
        score_ += 500/*300 * level*/;
    else if (lineGain->lineCount == 4)
        score_ += 800/*1200 * level*/;

    delete lineGain;
    lineGain = NULL;

    if(levelEndMark_ || !curBlock.checkBlockPosition())
    {
        levelEndMark_ = false;
        scorePerLevel_.push_back(score_);
        linesPerLevel_.push_back(lines_);
        ++level_;
        if(newLevelPause_)
            setPaused(true);
        else
            startNewLevel();
    }
}

FbgGame::~FbgGame()
{
    SaveLogFile();
    SaveScoreLogFile();
}

void FbgGame::processEvent(const SDL_Event & event)
{
    switch (getState())
    {
    case GAMEPLAY:
        switch (event.type)
        {
        case SDL_USEREVENT:
            if(event.user.code == FBG_EVENT_DROP)
                if (!getPaused())
                {
                     if (!getCurBlock()->moveBlockDown())
                     {
                        finishCurBlock();
                     }
                }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
            case SDLK_DOWN:
                if (!getPaused())
                {
                    downHeld_ = false;
                }
                break;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_DOWN:
                if (!getPaused())
                {
                    downHeld_ = true;
                }
                break;
            case SDLK_RIGHT:
                if (!getPaused())
                {
                     getCurBlock()->moveBlockRight();
                }
                break;
            case SDLK_LEFT:
                if (!getPaused())
                {
                    getCurBlock()->moveBlockLeft() ;
                }
                break;
            case SDLK_UP:
                if (!getPaused())
                {
                    getCurBlock()->rotateBlockRight();
                }
                break;
            }
            break;
        case SDL_QUIT:
            setState(QUIT);
            break;
        }
        break;
    default:
        switch (event.type)
        {
        case SDL_QUIT:
            setState(QUIT);
            break;
        }
        break;
    }
}

void FbgGame::startNewLevel()
{
    if(state_ == GAMEOVER)
        return;

    if(lineGain)
        delete lineGain;
    lineGain = NULL;

    //downStart = -1;
    lines_ = 0;
    score_ = 0;

    for (int row = 0; row < 18; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            matrix_[row][col] = 0;
        }
    }

    if(blockFactory_.IsLevelEnd(level_))
    {
        setState(GAMEOVER);
        ConfigMgr config;
        if(config.GetIntegerValue(mode_ + ".single_mode"))
        {
            lines_   = getTotalLines();
            score_ = getTotalScore();
        }
        return;
    }
    else
    {
        // 增加保存用户输入的数组
        vector< MoveState > curLevelMove;
        moveStates_.push_back(curLevelMove);

        setState(GAMEPLAY);

        blockFactory_.SetLevel(level_);
        nextBlockIndex = blockFactory_.GetNextBlock();
        cycleBlocks();
    }
}

void FbgGame::SaveScoreLogFile()
{
    ConfigMgr config;
    if(config.GetIntegerValue(mode_ + ".enable_score_log"))
    {
        scoreLog_ << "Total Score:" << getTotalScore() << "\n";
        scoreLog_ << "Total Lines:" << getTotalLines() << "\n";
        for(size_t i = 0; i < linesPerLevel_.size() && i < scorePerLevel_.size(); ++i)
        {
            scoreLog_ << "Level:" << i  << ", Score:" << scorePerLevel_[i] << ", Lines:" << linesPerLevel_[i] << "\n";
        }

        char base = '0';
        base += getID();
        string fileName = "log/score";
        // 单重游戏模式下输出到log/log.txt
        if( !config.GetIntegerValue(mode_ + ".single_mode") )
        {
            fileName += '_';
            fileName += base;
        }
        fileName += ".txt";
        scoreLog_.save_to(fileName.c_str());
    }
}

void FbgGame::SaveLogFile()
{
    ConfigMgr config;
    if(config.GetIntegerValue(mode_ + ".enable_log"))
    {
        log_ << "#Level Num\n";
        log_ << moveStates_.size() << '\n';
        for(int i = 0; i < (int)moveStates_.size(); ++i)
        {
            log_ << "#level " << i << '\n';
            log_ << moveStates_[i].size() << '\n';
            for(int j = 0; j < (int)moveStates_[i].size(); ++j)
            {
                log_ << moveStates_[i][j].move << ',' << moveStates_[i][j].rotate << '\n';
            }
        }

        char base = '0';
        base += getID();
        string fileName = "log/";
        // 单重游戏模式下输出到log/log.txt
        if( config.GetIntegerValue(mode_ + ".single_mode") )
            fileName += "log";
        else
            fileName += base;
        fileName += ".txt";
        log_.save_to(fileName.c_str());
    }
}

long FbgGame::getTotalScore()
{
    int sum = 0;
    for(size_t i = 0; i < scorePerLevel_.size(); ++i)
        sum += scorePerLevel_[i];
    if(!blockFactory_.IsLevelEnd(level_))
        sum += score_;
    return sum;
}

int FbgGame::getTotalLines()
{
    int sum = 0;
    for(size_t i = 0; i < linesPerLevel_.size(); ++i)
        sum += linesPerLevel_[i];
    if(!blockFactory_.IsLevelEnd(level_))
        sum += lines_;
    return sum;
}