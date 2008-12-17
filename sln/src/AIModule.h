#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include "TetrisAI.h"

class AIModule
{
public:
    AIModule();
    virtual ~AIModule();
    virtual bool Init(std::string mode, int id);
    virtual MoveState GetMoveStrategy( bool cur_block[4][4], bool next_block[4][4], int matrix[18][10] );
    MoveState DummyMoveState();
private:
    AIModule *module_;
};

class DllAIModule
    :public AIModule
{
public:
    DllAIModule();
    virtual ~DllAIModule();
    virtual bool Init(std::string mode, int id);
    virtual MoveState GetMoveStrategy( bool cur_block[4][4], bool next_block[4][4], int matrix[18][10] );
private:
    typedef MoveState (*AIFunc)(bool[4][4], bool[4][4], int[18][10]);
    HMODULE library_;
    AIFunc  function_;
};

class TextAIModule
    :public AIModule
{
public:
    TextAIModule();
    virtual ~TextAIModule();
    virtual bool Init(std::string mode, int id);
    virtual MoveState GetMoveStrategy( bool cur_block[4][4], bool next_block[4][4], int matrix[18][10] );
private:
    std::vector< std::vector< MoveState > > moveState_;
    int level_;
    int index_;
};