#pragma  once
#include "TetrisAIAPI.h"

struct MoveState   //需要返回的移动信息
{
    int move;     //水平移动步数,规定向左为正，向右为负
    int rotate;   //旋转角度 90*rotate，规定顺时针还是逆时针；
};

// 游戏区域的宽度(按格数计)
#define BOARD_WIDTH  (10)
// 游戏区域的高度
#define BOARD_HEIGHT (18)

void Init();


/************************************************************************/
/* 
函数说明：取得Block移动策略；
参数：    cur_block:当前Block；next_block:下一个Block;
          matrix_arr:目前矩阵
返回值：  MoveState:参考上述描述
*/
/************************************************************************/
extern "C" TETRISAI_API MoveState MoveStrategy(bool cur_block[4][4], bool next_block[4][4],int matrix_arr[18][10]);