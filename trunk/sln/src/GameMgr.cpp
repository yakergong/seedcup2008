#include "GameMgr.h"
#include "ConfigMgr.h"
#include "FbgRenderer.h"

using namespace std;

GameMgr GameMgr::instance_;

bool GameMgr::Init(bool idleMode,int startLevel, string mode )
{
    isIdleMode_ = idleMode;
    gameOver_ = false;
    paused_ = false;

    ConfigMgr config;

    if( config.GetIntegerValue(mode + ".speed_up_mode"))
        speedUpMode_ = true;
    else
        speedUpMode_ = false;

    int gameNum;
    int team_num = config.GetIntegerValue("global.team_num");
    if(config.GetIntegerValue(mode + ".single_mode"))
        gameNum = 1;
    else
        gameNum = team_num;

    for(int i = 0; i < gameNum; ++i)
    {
        FbgGame *game = new FbgGame(i, idleMode, startLevel, mode);
        game->setState(GAMEPLAY);
        game->setPaused(paused_);
        game->setSpeedUpMode(speedUpMode_);
        gameVec_.push_back(game);
    }

    if(mode == "show_log")
        normalDelay_ = config.GetIntegerValue("global.show_delay");
    else
        normalDelay_ = config.GetIntegerValue("global.normal_delay");
    tinyDelay_      = config.GetIntegerValue("global.tiny_delay");
    fps_ = config.GetIntegerValue("global.frame_per_second");
    downHeldSpeed_ = config.GetIntegerValue("global.down_held_speed");

    frameIndex_ = 0;
    ResetMod();

    newLevelPause_ = config.GetIntegerValue(mode + ".new_level_pause");

    return true;
}

GameMgr::GameMgr()
{

}

GameMgr::~GameMgr()
{
    Destroy();
}

void GameMgr::Loop()
{
        SDL_Event event;
        while( !gameOver_ )
        {
            int startTime = SDL_GetTicks();
            FbgRenderer::Instance().draw();
            // Event Handling
            while (SDL_PollEvent(&event))
            {
                if( CheckKeyEvent(event) )
                    continue;

                for(int i = 0; i < (int)gameVec_.size(); ++i)
                    gameVec_[i]->processEvent(event);
            }

            // 演示模式下每关暂停
            bool startFlag = true;
            for(int i = 0; i < (int)gameVec_.size(); ++i)
                if(gameVec_[i]->getPaused() == false)
                    startFlag = false;
            if(startFlag && newLevelPause_ && !paused_)
            for(int i = 0; i < (int)gameVec_.size(); ++i)
                gameVec_[i]->startNewLevel();

            // 调节速度
            if(gameVec_[0]->getDownHeld())
            {
                int newMod = mod_/downHeldSpeed_;
                if(newMod < 1)
                    newMod = 1;
                int times = 1;
                if( speedUpMode_ )
                    times = int(float(1000/fps_)/float(tinyDelay_));
                if(times < 1)
                    times = 1;
                if(frameIndex_%newMod == 0)
                    DropBlockFunc(times);
            }
            else
            {
                int times = 1;
                if( speedUpMode_ )
                    times = int(float(1000/fps_)/float(tinyDelay_));
                if(times < 1)
                    times = 1;
                if(frameIndex_ % mod_ == 0)
                    DropBlockFunc(times);
            }

            ++frameIndex_;

            // 保持帧率为fps_
            int endTime = SDL_GetTicks();
            int timePerFrame = 1000/fps_;
            if(endTime - startTime < timePerFrame)
            {
                SDL_Delay(timePerFrame - (endTime -startTime));
            }
        }
}

vector< FbgGame * >& GameMgr::GetGameVec()
{
    return gameVec_;
}

bool GameMgr::CheckKeyEvent( SDL_Event &event )
{
    if( event.type == SDL_QUIT )
    {
        gameOver_ = true;
        return true;
    }

    if(event.type == SDL_KEYDOWN)
    {
        SDLKey key = event.key.keysym.sym;
        if(key == SDLK_ESCAPE)
        {
            gameOver_ = true;
            return true;
        }
        else if(key == SDLK_F2)
        {
            paused_ = !paused_;
            for(int i = 0; i < (int)gameVec_.size(); ++i)
                gameVec_[i]->setPaused(paused_);
        }
        else if(key == SDLK_F3)
        {
            speedUpMode_ = !speedUpMode_;
            ResetMod();

            for(int i = 0; i < (int)gameVec_.size(); ++i)
                gameVec_[i]->setSpeedUpMode(speedUpMode_);
        }
    }

    return false;
}

bool GameMgr::IsGameOver()
{
    return gameOver_;
}

GameMgr& GameMgr::Instance()
{
    return instance_;
}

void GameMgr::Destroy()
{
    for(int i = 0; i < (int)gameVec_.size(); ++i)
    {
        delete gameVec_[i];
        gameVec_[i] = NULL;
    }
}

void GameMgr::DropBlockFunc(int times)
{
    SDL_Event event;
    SDL_UserEvent userEvent;

    userEvent.type  = SDL_USEREVENT;
    userEvent.code = FBG_EVENT_DROP;
    userEvent.data1 = NULL;
    userEvent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userEvent;

    for(int i = 0; i < times; ++i)
        SDL_PushEvent(&event);
}

void GameMgr::ResetMod()
{
    if(speedUpMode_)
        mod_ = tinyDelay_/fps_;
    else
        mod_ = normalDelay_/fps_;

    if(mod_ < 1)
        mod_ = 1;
}