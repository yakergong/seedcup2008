#include "BlockFactory.h"
#include "NumericConverter.h"
#include "ConfigMgr.h"
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

void BlockFactory::Init( string mode )
{
    level_ = 0;
    index_ = 0;

    ConfigMgr config;
    if( config.GetIntegerValue(mode + ".use_map_file") )
        useMapFile_ = true;
    else
        useMapFile_ = false;

    if(useMapFile_)
    {
        fstream file("data/map.txt", ios_base::in);
        string str;
        getline(file, str);
        // skip comment
        while(str.length() == 0 || (str.length() > 0 && str[0] == '#'))
            if(!getline(file,str))
                break;
        int levelNum;
        NumericConverter::GetValue(levelNum, str.c_str());
        for(int i = 0; i < levelNum; ++i)
        {
            std::vector<int> curLevel;
            getline(file, str);
            // skip comment
            while(str.length() == 0 || (str.length() > 0 && str[0] == '#'))
                if(!getline(file,str))
                    break;

            int elementNum;
            NumericConverter::GetValue(elementNum, str.c_str());

            getline(file, str);
            // skip comment
            while(str.length() == 0 || (str.length() > 0 && str[0] == '#'))
                if(!getline(file,str))
                    break;

            for(int j = 0; j < elementNum; ++j)
            {
                curLevel.push_back(str[2*j]-'0');
            }
            // dummy
            curLevel.push_back(0);

            blocks_.push_back( curLevel );
        }
    }
    else
    {
        srand((unsigned int)time(NULL));
    }
}

void BlockFactory::SetLevel( int level )
{
    level_ = level;
    index_ = 0;
}

int BlockFactory::GetLevelNum()
{
    return (int)blocks_.size();
}

int BlockFactory::GetNextBlock()
{
    if(useMapFile_)
    {
        ++index_;
        return blocks_[level_][index_ - 1];
    }
    else
    {
        return rand()%7;
    }
}

bool BlockFactory::IsThisLevelEnd()
{
    if(useMapFile_)
        return index_ >= (int)blocks_[level_].size();
    else
        return false;
}

bool BlockFactory::IsLevelEnd( int curLevel )
{
    if(useMapFile_)
        return curLevel >= (int)blocks_.size();
    else
        return false;
}