#pragma once

#include <vector>
#include <string>
#include <SDL/SDL.h>
#include "FbgGame.h"

class GameMgr
{
public:
    bool Init(bool idleMode, int startLevel, std::string mode);
    void Destroy();
    virtual ~GameMgr();

    static GameMgr& Instance();
    // 返回游戏数组
    std::vector< FbgGame * >& GetGameVec();
    // 游戏主循环
    void Loop();
    bool IsGameOver();
private:
    bool isIdleMode_;
    bool gameOver_;
    bool paused_;
    bool speedUpMode_;
    std::vector< FbgGame * > gameVec_;
    // 正常时延下落(以ms计)
    int normalDelay_;
    // 短时延下落(以ms计)
    int tinyDelay_;
    // 当前是第几帧
    int frameIndex_;
    // 帧频
    int fps_;
    int mod_;
    int downHeldSpeed_;
    // 是否在新的一关里暂停
    int newLevelPause_;

    GameMgr();
    static GameMgr instance_;

    // 预处理消息
    // @retval true 不再将消息发送到游戏中
    bool CheckKeyEvent(SDL_Event &event);

    void ResetMod();
    // 定时消去方块
    void DropBlockFunc(int times);
};