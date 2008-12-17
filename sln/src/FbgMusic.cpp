#include "FbgMusic.h"
#include "GameMgr.h"
#include <physfs/physfs.h>

#define FBGGAMESOUND_DELAY 10

using namespace std;

FbgMusic FbgMusic::instance_;

FbgMusic& FbgMusic::Instance()
{
    return instance_;
}

FbgMusic::FbgMusic()
{

}

void FbgMusic::initSound()
{
    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();
    MikMod_Init();

    music_ = NULL;
    musicID_ = NULL;
}

bool FbgMusic::initMusic(const string & musicFile, GameMgr *manager)
{
    // if (!MikMod_InitThreads()) return;  // Don't need threads, will use SDL when needed

    //modified by yaker
    //TODO:close memory file here
    if( PHYSFS_exists((string("music/") + musicFile).c_str()) )
    {
        //physfsMREADER* reader = getPhysfsMREADER(musicFile);
        //readerAddress = ((void*)(reader));
        //music = Player_LoadGeneric(((MREADER*)(reader)), 16, 0);
        music_ = Player_Load((char*)(string("data/music/") + musicFile).c_str(), 16, 0);

        Player_Start(music_);
        musicID_ = SDL_CreateThread(musicThread, manager);
        return true;
    }
    else
        return false;
}

void FbgMusic::stopMusic()
{
    if (musicID_)
        SDL_WaitThread(musicID_, NULL);

    if (music_ != NULL)
    {
        Player_Stop();
        Player_Free(music_);
        //freePhysfsMREADER(((physfsMREADER*)(readerAddress)));
    }
}
void FbgMusic::stopSound()
{
    MikMod_Exit();
}

// Main Music functions
int FbgMusic::musicThread(void *param)
{
    while ( false == ((GameMgr *) (param))->IsGameOver() )
    {
        //SDL_Delay(FBGGAMESOUND_DELAY);
        if (Player_Active())
        {
            // Locking shouldn't really be necessary for now
            MikMod_Update();
        }
    }
    return 0;
}
