#include "FbgGameMode.h"

using namespace std;

FbgGameMode ToGameMode( string mode )
{
    if(mode == "contest")
        return GAME_CONTEST;
    else if(mode == "gene_log")
        return GAME_GENE_LOG;
    else if(mode == "show_log")
        return GAME_SHOW_LOG;
    else
        return GAME_NORMAL;
}