#pragma once

#include <string>

enum FbgGameMode
{
    GAME_NORMAL,
    GAME_CONTEST,
    GAME_GENE_LOG,
    GAME_SHOW_LOG
};

FbgGameMode ToGameMode( std::string mode );