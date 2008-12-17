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
    // ������Ϸ����
    std::vector< FbgGame * >& GetGameVec();
    // ��Ϸ��ѭ��
    void Loop();
    bool IsGameOver();
private:
    bool isIdleMode_;
    bool gameOver_;
    bool paused_;
    bool speedUpMode_;
    std::vector< FbgGame * > gameVec_;
    // ����ʱ������(��ms��)
    int normalDelay_;
    // ��ʱ������(��ms��)
    int tinyDelay_;
    // ��ǰ�ǵڼ�֡
    int frameIndex_;
    // ֡Ƶ
    int fps_;
    int mod_;
    int downHeldSpeed_;
    // �Ƿ����µ�һ������ͣ
    int newLevelPause_;

    GameMgr();
    static GameMgr instance_;

    // Ԥ������Ϣ
    // @retval true ���ٽ���Ϣ���͵���Ϸ��
    bool CheckKeyEvent(SDL_Event &event);

    void ResetMod();
    // ��ʱ��ȥ����
    void DropBlockFunc(int times);
};