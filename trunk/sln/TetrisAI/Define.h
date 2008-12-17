#pragma once

struct MoveState   //��Ҫ���ص��ƶ���Ϣ
{
    int move;     //ˮƽ�ƶ�����,�涨����Ϊ��������Ϊ��
    int rotate;   //��ת�Ƕ� 90*rotate���涨˳ʱ�뻹����ʱ�룻
};

enum Block
{
    BLOCK_I = 0,
    BLOCK_O,
    BLOCK_L,
    BLOCK_J,
    BLOCK_T,
    BLOCK_S,
    BLOCK_Z
};

// ����ı߳�
#define BLOCK_SIDE_LEN (4)
typedef bool BlockMatrix[4][4];

// ��Ϸ����Ŀ��(��������)
#define BOARD_WIDTH  (10)
// ��Ϸ����ĸ߶�
#define BOARD_HEIGHT (18)
typedef bool BoardMatrix[18][10];

// 
// O
// {
//     bool tmp[16] = { false, true, true, false,
//         false, true, true, false,
//         false, false, false, false,
//         false, false, false, false
//     };
// }
// I
// {
//     bool tmp[16] = { true, true, true, true,
//         false, false, false, false,
//         false, false, false, false,
//         false, false, false, false
//     };
// }
// L
// {
//     bool tmp[16] = { true, true, true, false,
//         true, false, false, false,
//         false, false, false, false,
//         false, false, false, false
//     };
// }
// J
// {
//     bool tmp[16] = { true, true, true, false,
//         false, false, true, false,
//         false, false, false, false,
//         false, false, false, false
//     };
// }
// S
// {
//     bool tmp[16] = { false, true, true, false,
//         true, true, false, false,
//         false, false, false, false,
//         false, false, false, false
//     };
// }
// Z
// {
//     bool tmp[16] = { true, true, false, false,
//         false, true, true, false,
//         false, false, false, false,
//         false, false, false, false
//     };
// }
// T
// {
//     bool tmp[16] = { true, true, true, false,
//         false, true, false, false,
//         false, false, false, false,
//         false, false, false, false
//     };
// }