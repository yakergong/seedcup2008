#pragma once

struct MoveState   //需要返回的移动信息
{
    int move;     //水平移动步数,规定向左为负，向右为正
    int rotate;   //旋转角度 90*rotate，规定顺时针还是逆时针；
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

// 方块的边长
#define BLOCK_SIDE_LEN (4)
typedef bool BlockMatrix[4][4];

// 游戏区域的宽度(按格数计)
#define BOARD_WIDTH  (10)
// 游戏区域的高度
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