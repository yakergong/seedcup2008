#include "TetrisAIAPI.h"
#include "TetrisAI.h"
#include "Block.h"

// �����Ҫ��ʼ�������ڴ����
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

