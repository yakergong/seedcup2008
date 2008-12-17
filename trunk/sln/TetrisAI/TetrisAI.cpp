#include "TetrisAIAPI.h"
#include "TetrisAI.h"
#include "Block.h"

// 如果需要初始化代码在此添加
void Init()
{

}

extern "C" TETRISAI_API MoveState MoveStrategy(bool cur_block[4][4], bool next_block[4][4],int matrix_arr[18][10])
{
    MoveState movement;
    movement.move = 0;
    movement.rotate = 0;
    return movement;
}

