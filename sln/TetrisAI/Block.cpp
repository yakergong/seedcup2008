

#include "Block.h"

short GetBlockLeftX(bool matrix[4][4])
{
    int col;
    for (col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            if (matrix[row][col])
                return col;
        }
    }
    return -1;
}


short  GetBlockRightX(bool matrix[4][4])
{
    int col;
    for (col = 3; col >= 0; col--)
    {
        for (int row = 0; row < 4; row++)
        {
            if (matrix[row][col])
                return col;
        }
    }
    return -1;
}

short  GetBlockTopY(bool matrix[4][4])
{
    int row;
    for (row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (matrix[row][col])
                return row;
        }
    }
    return -1;
}

short  GetBlockBottomY(bool matrix[4][4])
{
    int row;
    for (row = 3; row >= 0; row--)
    {
        for (int col = 0; col < 4; col++)
        {
            if (matrix[row][col])
                return row;
        }
    }
    return -1;
}

short   GetBlockWidth(bool matrix[4][4])
{
    return GetBlockRightX(matrix) - GetBlockLeftX(matrix) + 1;
}

short   GetBlockHeight(bool matrix[4][4])
{
    return GetBlockBottomY(matrix) - GetBlockTopY(matrix) + 1;
}

void  shiftLeft(bool matrix[4][4])
{
    for (int col = 1; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
            matrix[row][col - 1] = matrix[row][col];
    }
    for (int row = 0; row < 4; row++)
        matrix[row][3] = false;
}

void  shiftRight(bool matrix[4][4])
{
    for (int col = 2; col >= 0; col--)
    {
        for (int row = 0; row < 4; row++)
            matrix[row][col + 1] = matrix[row][col];
    }
    for (int row = 0; row < 4; row++)
        matrix[row][0] = false;
}

void  shiftUp(bool matrix[4][4])
{
    for (int row = 1; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            matrix[row - 1][col] = matrix[row][col];
    }
    for (int col = 0; col < 4; col++)
        matrix[3][col] = false;
}

void  shiftDown(bool matrix[4][4])
{
    for (int row = 2; row >= 0; row--)
    {
        for (int col = 0; col < 4; col++)
            matrix[row + 1][col] = matrix[row][col];
    }
    for (int col = 0; col < 4; col++)
        matrix[0][col] = false;
}

void  shift(bool matrix[4][4],int targetX, int targetY)
{
    int x = GetBlockLeftX(matrix);
    int y = GetBlockTopY(matrix);
    int width = GetBlockWidth(matrix);
    int height = GetBlockHeight(matrix);

    // Move to Top
    while (y > 0)
    {
        shiftUp(matrix);
        y--;
    }
    // Move to old level when possible
    while (y < targetY)
    {
        if (GetBlockBottomY(matrix) == 3)
            break;
        shiftDown(matrix);
        y++;
    }
    // Center at X-target when possible
    while (x != targetX)
    {
        if (x < targetX)
        {
            if (GetBlockRightX(matrix) == 3)
                break;
            shiftRight(matrix);
            x++;
        }
        else if (x > targetX)
        {
            if (GetBlockLeftX(matrix) == 0)
                break;
            shiftLeft(matrix);
            x--;
        }
    }
}

//Ðý×ª
void  rotateRight(bool matrix[4][4])
{
    int oldX2 = GetBlockRightX(matrix);
    int oldY = GetBlockTopY(matrix);
    bool oldMatrix[4][4];
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            oldMatrix[row][col] = matrix[row][col];
    }
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            matrix[row][col] = oldMatrix[3 - col][row];
    }
    shift(matrix,oldX2 - GetBlockWidth(matrix) + 1, oldY);
}

void RotateBlock(bool block[4][4],int rotateStep)
{
    for (int i = rotateStep; i > 0; --i)
    {
        rotateRight(block);
    }
}