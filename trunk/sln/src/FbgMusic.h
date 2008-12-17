#include <mikmod/mikmod.h>

#include <string>
#include <sdl/SDL.h>

class GameMgr;

class FbgMusic
{
public:
    static  FbgMusic& Instance();
    void initSound();
    bool initMusic(const std::string & musicFile, GameMgr *manager);
    void stopMusic();
    void stopSound();
    static int musicThread(void *param);
private:
    FbgMusic();

    static FbgMusic instance_;

    // Music
    //void   *readerAddress_;
    MODULE *music_;
    SDL_Thread *musicID_;
};