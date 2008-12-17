#include "AIModule.h"
#include "ConfigMgr.h"
#include "NumericConverter.h"
#include <fstream>

using namespace std;

AIModule::AIModule()
{
    module_ = NULL;
}

AIModule::~AIModule()
{
    if(module_)
        delete module_;
}

bool AIModule::Init( std::string mode, int id )
{
    string inputType = ConfigMgr().GetStringValue(mode + ".input_type");
    if(inputType != "text")
    {
        module_ = new DllAIModule;
        return module_->Init(mode, id);
    }
    else
    {
        module_ = new TextAIModule;
        return module_->Init(mode, id);
    }
}

MoveState AIModule::GetMoveStrategy( bool cur_block[4][4], bool next_block[4][4], int matrix[18][10] )
{
    if(module_)
        return module_->GetMoveStrategy(cur_block, next_block, matrix);
    else
    {
        MoveState state;
        state.move = 0;
        state.rotate = 0;
        return state;
    }
}

MoveState AIModule::DummyMoveState()
{
    MoveState state;
    state.move   = 0;
    state.rotate = 0;
    return state;
}

DllAIModule::DllAIModule()
{
    library_  = NULL;
    function_ = NULL;
}

bool DllAIModule::Init( std::string mode, int id )
{
    string inputType = ConfigMgr().GetStringValue(mode + ".input_type");
    wstring dllName;
    if(inputType == "dll")
        dllName = TEXT("TetrisAI.dll");
    else if(inputType == "mul_dll")
    {
        dllName += TEXT("data/");
        dllName += NumericConverter::to_wstr(id);
        dllName += TEXT(".dll");
    }
    library_  = ::LoadLibrary(dllName.c_str());
    if(library_ != NULL)
    {
        function_ = (AIFunc)::GetProcAddress(library_, "MoveStrategy");
        if(function_ == NULL)
            return false;
        else
            return true;
    }
    else
        return false;
}

DllAIModule::~DllAIModule()
{
    if(library_ != NULL)
        ::FreeLibrary(library_);
}

MoveState DllAIModule::GetMoveStrategy( bool cur_block[4][4], bool next_block[4][4], int matrix[18][10] )
{
    if(function_)
        return function_(cur_block, next_block, matrix);
    else
        return MoveState();
}

TextAIModule::TextAIModule()
{
    level_ = 0;
    index_ = 0;
}

TextAIModule::~TextAIModule()
{

}

bool TextAIModule::Init( std::string mode, int id )
{
    string fileName = "log/";
    char base = '0';
    base += id;
    fileName += base;
    fileName += ".txt";

    fstream file(fileName.c_str(), ios_base::in);
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
        std::vector<MoveState> curLevel;
        getline(file, str);
        // skip comment
        while(str.length() == 0 || (str.length() > 0 && str[0] == '#'))
            if(!getline(file,str))
                break;

        int elementNum;
        NumericConverter::GetValue(elementNum, str.c_str());

        MoveState state;
        for(int j = 0; j < elementNum; ++j)
        {
            getline(file, str);
            int k;
            for(k = 0; k < (int)str.length(); ++k)
                if(str[k] == ',')
                    break;
            NumericConverter::GetValue(state.move   , str.substr(0, k).c_str());
            NumericConverter::GetValue(state.rotate , str.substr(k+1).c_str());

            curLevel.push_back(state);
        }

        moveState_.push_back( curLevel );
    }

    return true;
}

MoveState TextAIModule::GetMoveStrategy( bool cur_block[4][4], bool next_block[4][4], int matrix[18][10] )
{
    if(level_ == (int)moveState_.size())
        return DummyMoveState();

    if(index_ >= (int)moveState_[level_].size())
    {
        index_ = 0;
        ++level_;
        if(level_ == (int)moveState_.size())
            return DummyMoveState();
        else
        {
            ++index_;
            return moveState_[level_][index_ - 1];
        }
    }
    else
    {
        ++index_;
        return moveState_[level_][index_ - 1];
    }
}