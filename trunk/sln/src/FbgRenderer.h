#ifndef FBG_RENDERER_H
#define FBG_RENDERER_H

#include <string>
#include "glTGAImage.h"
#include "fbgblock.h"

class FbgGame;

class FbgRenderer
{
public:
    static FbgRenderer& Instance();
    bool init(bool fs = false, int width = 640, int height = 480, int bpp = 24);
    void exit();

    void draw();

private:
    FbgRenderer();
    static FbgRenderer instance_;

    // 增加图片
    GLTgaImage msb_;

    // Textures
    GLTgaImage *bg_;
    GLTgaImage *ml_;
    GLTgaImage *mb_;
    GLTgaImage *mt_;
    GLTgaImage *nb_;
    GLTgaImage *gameover_;
    GLTgaImage nr_;
    GLTgaImage *blocks_;
    GLTgaImage *digits_;
    // Display Lists
    GLuint  cube_;
    GLuint  cubeWithoutBack_;
    GLuint  digitsBase_;
    GLfloat lineZSpeeds_[4][10];

    void loadTextures();
    void killTextures();
    void buildLists();                // Build OpenGL display lists
    void killLists();                 // Destroy OpenGL display lists

    // 绘制背景，由draw调用
    void drawBkgnd(FbgGame *game);
    void drawGameID(FbgGame *game);
    void drawMulGameInfo(FbgGame *game);
    void drawCurBlockFrame(FbgGame *game);
    void drawNextBlockFrame(FbgGame *game);
    void drawGameInfo(FbgGame *game);
    void drawGameplay(FbgGame *game);              // Normal rendering during game play

    void drawBlock(FbgGame *game, FbgBlock * theBlock);
    void drawCurBlock( FbgGame * game );
    void drawNextBlock( FbgGame * game );

    void drawLineGain(FbgGame *game);              // 绘制一行逐渐被消去的过程
    void drawGameOver(FbgGame *game);              // When the game ends
    void drawGameMatrix(FbgGame *game);            // Code-saving function for drawing the game matrix
    void drawDigits(const std::string & str);
};

#endif