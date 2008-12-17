#pragma once

#include <string>
#include <vector>

class BlockFactory
{
public:
    void Init(std::string mode);
    void SetLevel(int level);
    int  GetLevelNum();
    int  GetNextBlock();
    bool IsThisLevelEnd();
    bool IsLevelEnd(int curLevel);
private:
    std::vector< std::vector<int> > blocks_;
    bool useMapFile_;
    int level_;
    int index_;
};