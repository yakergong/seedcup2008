/*
 * Falling Block Game
 * Copyright (C) 1999-2002 Jared Krinke <http://derajdezine.vze.com/>
 * 
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 or (at your option) any later version
 * as published by the Free Software Foundation.
 * 
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this distribution; if not, write to:
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307  USA
 * 
 * Jared Krinke
 * 
 * Deraj DeZine
 * http://derajdezine.vze.com/
 */

#include "FbgBlock.h"
#include "FbgGame.h"

short FbgBlock::getX()
{
    int col;
    for (col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            if (matrix_[row][col])
                return col;
        }
    }
    return -1;
}

//获得矩阵旋转点
short FbgBlock::getX2()
{
    int col;
    for (col = 3; col >= 0; col--)
    {
        for (int row = 0; row < 4; row++)
        {
            if (matrix_[row][col])
                return col;
        }
    }
    return -1;
}

short FbgBlock::getY()
{
    int row;
    for (row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (matrix_[row][col])
                return row;
        }
    }
    return -1;
}

short FbgBlock::getY2()
{
    int row;
    for (row = 3; row >= 0; row--)
    {
        for (int col = 0; col < 4; col++)
        {
            if (matrix_[row][col])
                return row;
        }
    }
    return -1;
}

void FbgBlock::shiftLeft()
{
    for (int col = 1; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
            matrix_[row][col - 1] = matrix_[row][col];
    }
    for (int row = 0; row < 4; row++)
        matrix_[row][3] = false;
}

void FbgBlock::shiftRight()
{
    for (int col = 2; col >= 0; col--)
    {
        for (int row = 0; row < 4; row++)
            matrix_[row][col + 1] = matrix_[row][col];
    }
    for (int row = 0; row < 4; row++)
        matrix_[row][0] = false;
}

void FbgBlock::shiftUp()
{
    for (int row = 1; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            matrix_[row - 1][col] = matrix_[row][col];
    }
    for (int col = 0; col < 4; col++)
        matrix_[3][col] = false;
}

void FbgBlock::shiftDown()
{
    for (int row = 2; row >= 0; row--)
    {
        for (int col = 0; col < 4; col++)
            matrix_[row + 1][col] = matrix_[row][col];
    }
    for (int col = 0; col < 4; col++)
        matrix_[0][col] = false;
}

void FbgBlock::shift(int targetX, int targetY)
{
    int x = getX();
    int y = getY();
    int width = getWidth();
    int height = getHeight();

    // Move to Top
    while (y > 0)
    {
        shiftUp();
        y--;
    }
    // Move to old level when possible
    while (y < targetY)
    {
        if (getY2() == 3)
            break;
        shiftDown();
        y++;
    }
    // Center at X-target when possible
    while (x != targetX)
    {
        if (x < targetX)
        {
            if (getX2() == 3)
                break;
            shiftRight();
            x++;
        }
        else if (x > targetX)
        {
            if (getX() == 0)
                break;
            shiftLeft();
            x--;
        }
    }
}

void FbgBlock::rotateLeft()
{
    int oldX = getX();
    int oldY = getY();
    bool oldMatrix[4][4];
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            oldMatrix[row][col] = matrix_[row][col];
    }
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            matrix_[row][col] = oldMatrix[col][3 - row];
    }
    shift(oldX, oldY);
}

//旋转
void FbgBlock::rotateRight()
{
    int oldX2 = getX2();
    int oldY = getY();
    bool oldMatrix[4][4];
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            oldMatrix[row][col] = matrix_[row][col];
    }
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            matrix_[row][col] = oldMatrix[3 - col][row];
    }
    shift(oldX2 - getWidth() + 1, oldY);
}

FbgBlock::FbgBlock(FbgGame * newGame, short newIndex, bool newMatrix[16])
{
    game = newGame;
    index = newIndex;
    posX = 3;
    posY = 1;
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            matrix_[row][col] = newMatrix[row * 4 + col];
    }
}

FbgBlock::FbgBlock()
{
    posX = 3;
    posY = 1;
    game = NULL;
    index = 0;
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            matrix_[row][col] = false;
    }
}

void FbgBlock::operator=(const FbgBlock & theBlock)
{
    game = theBlock.getGame();
    posX = theBlock.getPosX();
    posY = theBlock.getPosY();
    index = theBlock.getIndex();
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            matrix_[row][col] = theBlock.getMatrixAt(row, col);
    }
}

bool FbgBlock::checkBlockPosition() const
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (matrix_[row][col] && game->getMatrixAt(posY + row, posX + col))
                return false;
        }
    }
    return true;
}

bool FbgBlock::moveBlockLeft()
{
    if (posX > 0)
    {
        posX--;
        if (checkBlockPosition())
            return true;
        posX++;
    }
    else if (getX() > 0)
    {
        shiftLeft();
        if (checkBlockPosition())
            return true;
        else
            shiftRight();
    }
    return false;
}

bool FbgBlock::moveBlockRight()
{
    if (posX < 6)
    {
        posX++;
        if (checkBlockPosition())
            return true;
        posX--;
    }
    else if (getX2() < 3)
    {
        shiftRight();
        if (checkBlockPosition())
            return true;
        else
            shiftLeft();
    }
    return false;
}

bool FbgBlock::moveBlockDown()
{
    posY++;
    if (posY >= 15 || !checkBlockPosition())
    {                                    // Nudging
        posY--;
        if (getY2() < 3)
        {
            shiftDown();
            if (checkBlockPosition())
                return true;
            else
                shiftUp();
        }
        return false;
    }
    else
        return true;
}

bool FbgBlock::moveBlockUp()
{
    posY--;
    if (posY < 0 || !checkBlockPosition())
    {                                    // Nudging
        posY++;
        if (getY() > 0)
        {
            shiftUp();
            if (checkBlockPosition())
                return true;
            else
                shiftDown();
        }
        return false;
    }
    else
        return true;
}

void FbgBlock::rotateBlockLeft()
{
    FbgBlock tmp = *this;
    tmp.rotateLeft();
    if (tmp.checkBlockPosition())
    {
        *this = tmp;
        return;
    }
    else
    {
        // Try left
        tmp.moveBlockLeft();
        if (tmp.checkBlockPosition())
        {
            *this = tmp;
            return;
        }
        // Try right
        tmp = *this;
        tmp.rotateLeft();
        tmp.moveBlockRight();
        if (tmp.checkBlockPosition())
        {
            *this = tmp;
            return;
        }
        // Try left x2
        tmp = *this;
        tmp.rotateLeft();
        tmp.moveBlockLeft();
        tmp.moveBlockLeft();
        if (tmp.checkBlockPosition())
        {
            *this = tmp;
            return;
        }
        // Try down
        tmp = *this;
        tmp.rotateLeft();
        tmp.moveBlockDown();
        if (tmp.checkBlockPosition())
        {
            *this = tmp;
            return;
        }
    }
}
void FbgBlock::rotateBlockRight()
{
    FbgBlock tmp = *this;
    tmp.rotateRight();
    if (tmp.checkBlockPosition())
    {
        *this = tmp;
        return;
    }
    else
    {
        // Try Right
        tmp.moveBlockRight();
        if (tmp.checkBlockPosition())
        {
            *this = tmp;
            return;
        }
        // Try left
        tmp = *this;
        tmp.rotateRight();
        tmp.moveBlockLeft();
        if (tmp.checkBlockPosition())
        {
            *this = tmp;
            return;
        }
        // Try right x2
        tmp = *this;
        tmp.rotateRight();
        tmp.moveBlockRight();
        tmp.moveBlockRight();
        if (tmp.checkBlockPosition())
        {
            *this = tmp;
            return;
        }
        // Try down
        tmp = *this;
        tmp.rotateRight();
        tmp.moveBlockDown();
        if (tmp.checkBlockPosition())
        {
            *this = tmp;
            return;
        }
    }
}

void FbgBlock::copyToMatrix( bool matrix[4][4] )
{
    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            if(matrix_[i][j])
                matrix[i][j] = true;
            else
                matrix[i][j] = false;
}