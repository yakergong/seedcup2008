#ifndef FBG_BLOCK_H
#define FBG_BLOCK_H

class FbgGame;

// Holds block's matrix and can perform rotating and such functions
class FbgBlock
{
public:
    FbgBlock(FbgGame * newGame, short newIndex, bool newMatrix[16]);    // Constructor
    FbgBlock();
    void operator=(const FbgBlock &);    // Copy

    void copyToMatrix(bool matrix[4][4]);
    // Block Management
    bool checkBlockPosition() const;
    bool moveBlockDown();            // Returns if block was actually moved
    bool moveBlockRight();
    bool moveBlockLeft();
    bool moveBlockUp();
    void rotateBlockRight();
    void rotateBlockLeft();

    FbgGame *getGame() const
    {
        return game;
    }

    short getX();                        // From left where block starts
    short getY();                        // From top where block starts
    short getX2();                    // From right, where block stats
    short getY2();                    // From bottom, where block starts

    short getWidth()
    {
        return getX2() - getX() + 1;
    }

    short getHeight()
    {
        return getY2() - getY() + 1;
    }

    short getPosX() const
    {
        return posX;
    }

    short getPosY() const
    {
        return posY;
    }

    void setPosX(short newX)
    {
        posX = newX;
    }
    void setPosY(short newY)
    {
        posY = newY;
    }

    short getIndex() const
    {
        return index;
    }

    bool getMatrixAt(int row, int col) const
    {
        return matrix_[row][col];
    }
private:
    bool matrix_[4][4];
    short   posX, posY;                    // Position within game
    FbgGame *game;
    short   index;                        // Blocks color/pattern

    // Matrix Management
    void shiftLeft();
    void shiftRight();
    void shiftUp();
    void shiftDown();
    void shift(int targetX, int targetY);    // Center horizontally and move to top

    void rotateLeft();                // Actually moves bricks in matrix
    void rotateRight();
};

#endif