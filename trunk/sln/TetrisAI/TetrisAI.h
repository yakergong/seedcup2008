#pragma  once
#include "TetrisAIAPI.h"

struct MoveState   //��Ҫ���ص��ƶ���Ϣ
{
    int move;     //ˮƽ�ƶ�����,�涨����Ϊ��������Ϊ��
    int rotate;   //��ת�Ƕ� 90*rotate���涨˳ʱ�뻹����ʱ�룻
};

// ��Ϸ����Ŀ��(��������)
#define BOARD_WIDTH  (10)
// ��Ϸ����ĸ߶�
#define BOARD_HEIGHT (18)

void Init();


/************************************************************************/
/* 
����˵����ȡ��Block�ƶ����ԣ�
������    cur_block:��ǰBlock��next_block:��һ��Block;
          matrix_arr:Ŀǰ����
����ֵ��  MoveState:�ο���������
*/
/************************************************************************/
extern "C" TETRISAI_API MoveState MoveStrategy(bool cur_block[4][4], bool next_block[4][4],int matrix_arr[18][10]);