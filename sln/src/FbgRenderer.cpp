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

#include "glTGAImage.h"
#include "FbgBlock.h"
#include "FbgRenderer.h"
#include "FbgGame.h"
#include "FbgResource.h"
#include "GameMgr.h"
#include "ConfigMgr.h"

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>

FbgRenderer FbgRenderer::instance_;

using namespace std;

#define FBG_TITLE "Falling Block Game"

#pragma warning(disable:4996)

bool FbgRenderer::init(bool fs, int width, int height, int bpp)
{
	// Initialize SDL/OpenGL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
		return false;
	}
	SDL_WM_SetCaption(FBG_TITLE, NULL);
	if (fs)
		SDL_ShowCursor(0);

	// User didn't specify bpp
	if (bpp < 0)
		bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

	// Set bits per channel
	const unsigned char bpc = bpp < 32 ? 4 : 5;
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, bpc);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, bpc);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, bpc);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (SDL_SetVideoMode(width, height, bpp, (fs ? SDL_FULLSCREEN : 0) | SDL_OPENGL) == NULL)
	{
		cerr << "Unable to Create OpenGL Screen: " << SDL_GetError() << endl;
		return false;
	}

	glViewport(0, 0, width, height);

	// Setup OpenGL Viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, GLfloat(4) / GLfloat(3), 300.0f, 2000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	loadTextures();
	buildLists();

	// Setup OpenGL options (lighting, texturing, etc.)
	glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_FLAT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	GLfloat LightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[] = { -2.0f, 2.0f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LIGHTING);

	glEnable(GL_COLOR_MATERIAL);

	// Setup line gain speeds
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 10; col++)
			lineZSpeeds_[row][col] = float (rand() % 100 + 50) / 100 * 250;
	}

	return true;
}

void FbgRenderer::exit()
{
	killLists();
	// Quit SDL
	SDL_Quit();
}

void FbgRenderer::loadTextures()
{
    FbgResource& resource = FbgResource::Instance();

	GLTgaImage bgload, mlload, mbload, mtload, nbload, blocksload, digitsload, gameoverload;
	// Load Images
	if (   !bgload.loadTGAImage(resource.LoadThemeFile("bg.tga"))
		|| !mlload.loadTGAImage(resource.LoadThemeFile("ml.tga"))
		|| !mbload.loadTGAImage(resource.LoadThemeFile("mb.tga"))
		|| !mtload.loadTGAImage(resource.LoadThemeFile("mt.tga"))
		|| !nbload.loadTGAImage(resource.LoadThemeFile("nb.tga"))
		|| !nr_.loadTGAImage(resource.LoadThemeFile("nr.tga"))
		|| !blocksload.loadTGAImage(resource.LoadThemeFile("blockmap.tga"))
		|| !digitsload.loadTGAImage(resource.LoadThemeFile("digits.tga"))
		|| !gameoverload.loadTGAImage(resource.LoadThemeFile("gameover.tga"))
        || !msb_.loadTGAImage(resource.LoadThemeFile("msb.tga")) )
	{
		cout << "Could not load graphics" << endl;
		::exit(0);
	}

	// Setup Images
	bg_ = bgload.splitImageMap(256, 256, 12);
	for (int i = 0; i < 12; i++)
		bg_[i].makeClamped();

	mlload.cropImage(0, -24, 64, 768);
	ml_ = mlload.splitImageMap(64, 256, 3);
	for (int i = 0; i < 3; i++)
		ml_[i].makeClamped();
	mbload.cropImage(-24, 0, 512, 64);
	mb_ = mbload.splitImageMap(256, 64, 2);
	for (int i = 0; i < 2; i++)
		mb_[i].makeClamped();
	mtload.cropImage(-24, 0, 512, 64);
	mt_ = mtload.splitImageMap(256, 64, 2);
	for (int i = 0; i < 2; i++)
		mt_[i].makeClamped();

	nbload.cropImage(-32, 0, 512, 64);
	nb_ = nbload.splitImageMap(256, 64, 2);
	for (int i = 0; i < 2; i++)
		nb_[i].makeClamped();
	nr_.cropImage(0, -32, 64, 256);
	nr_.makeClamped();

	blocks_ = blocksload.splitImageMap(64, 64, 7);

	digits_ = digitsload.splitImageMap(64, 64, 10);
	for (int i = 0; i < 10; i++)
		digits_[i].makeClamped();

	gameover_ = gameoverload.splitImageMap(256, 256, 2);
	for (int i = 0; i < 2; i++)
		gameover_[i].makeClamped();
}

void FbgRenderer::killTextures()
{
	delete[] bg_;
	delete[] ml_;
	delete[] mb_;
	delete[] mt_;
	delete[] nb_;
	delete[] digits_;
	delete[] gameover_;
}

void FbgRenderer::buildLists() {
	cube_ = glGenLists(2);

	glNewList(cube_, GL_COMPILE);
// Back
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2d(1,1);glVertex3f(0.0f, 0.0f, -40.0f);
		glTexCoord2d(0,1);glVertex3f(40.0f, 0.0f, -40.0f);
		glTexCoord2d(1,0);glVertex3f(0.0f, -40.0f, -40.0f);
		glTexCoord2d(0,0);glVertex3f(40.0f, -40.0f, -40.0f);
	glEnd();
// Bottom
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2d(1,1);glVertex3f(40.0f, -40.0f, 0.0f);
		glTexCoord2d(0,1);glVertex3f(0.0f, -40.0f, 0.0f);
		glTexCoord2d(1,0);glVertex3f(40.0f, -40.0f, -40.0f);
		glTexCoord2d(0,0);glVertex3f(0.0f, -40.0f, -40.0f);
	glEnd();
// Top
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(1,1);glVertex3f(40.0f, 0.0f, -40.0f);
		glTexCoord2d(0,1);glVertex3f(0.0f, 0.0f, -40.0f);
		glTexCoord2d(1,0);glVertex3f(40.0f, 0.0f, 0.0f);
		glTexCoord2d(0,0);glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
// Left
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2d(1,1);glVertex3f(0.0f, 0.0f, 0.0f);
		glTexCoord2d(0,1);glVertex3f(0.0f, 0.0f, -40.0f);
		glTexCoord2d(1,0);glVertex3f(0.0f, -40.0f, 0.0f);
		glTexCoord2d(0,0);glVertex3f(0.0f, -40.0f, -40.0f);
	glEnd();
// Right
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2d(1,1);glVertex3f(40.0f, 0.0f, -40.0f);
		glTexCoord2d(0,1);glVertex3f(40.0f, 0.0f, 0.0f);
		glTexCoord2d(1,0);glVertex3f(40.0f, -40.0f, -40.0f);
		glTexCoord2d(0,0);glVertex3f(40.0f, -40.0f, 0.0f);
	glEnd();
// Front
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2d(1,1);glVertex3f(40.0f, 0.0f, 0.0f);
		glTexCoord2d(0,1);glVertex3f(0.0f, 0.0f, 0.0f);
		glTexCoord2d(1,0);glVertex3f(40.0f, -40.0f, 0.0f);
		glTexCoord2d(0,0);glVertex3f(0.0f, -40.0f, 0.0f);
	glEnd();
	glEndList();

	cubeWithoutBack_ = cube_+1;
	glNewList(cubeWithoutBack_, GL_COMPILE);
// Bottom
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2d(1,1);glVertex3f(40.0f, -40.0f, 0.0f);
		glTexCoord2d(0,1);glVertex3f(0.0f, -40.0f, 0.0f);
		glTexCoord2d(1,0);glVertex3f(40.0f, -40.0f, -40.0f);
		glTexCoord2d(0,0);glVertex3f(0.0f, -40.0f, -40.0f);
	glEnd();
// Top
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(1,1);glVertex3f(40.0f, 0.0f, -40.0f);
		glTexCoord2d(0,1);glVertex3f(0.0f, 0.0f, -40.0f);
		glTexCoord2d(1,0);glVertex3f(40.0f, 0.0f, 0.0f);
		glTexCoord2d(0,0);glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
// Left
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2d(1,1);glVertex3f(0.0f, 0.0f, 0.0f);
		glTexCoord2d(0,1);glVertex3f(0.0f, 0.0f, -40.0f);
		glTexCoord2d(1,0);glVertex3f(0.0f, -40.0f, 0.0f);
		glTexCoord2d(0,0);glVertex3f(0.0f, -40.0f, -40.0f);
	glEnd();
// Right
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2d(1,1);glVertex3f(40.0f, 0.0f, -40.0f);
		glTexCoord2d(0,1);glVertex3f(40.0f, 0.0f, 0.0f);
		glTexCoord2d(1,0);glVertex3f(40.0f, -40.0f, -40.0f);
		glTexCoord2d(0,0);glVertex3f(40.0f, -40.0f, 0.0f);
	glEnd();
// Front
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2d(1,1);glVertex3f(40.0f, 0.0f, 0.0f);
		glTexCoord2d(0,1);glVertex3f(0.0f, 0.0f, 0.0f);
		glTexCoord2d(1,0);glVertex3f(40.0f, -40.0f, 0.0f);
		glTexCoord2d(0,0);glVertex3f(0.0f, -40.0f, 0.0f);
	glEnd();
	glEndList();

	digitsBase_=glGenLists(10);
	for (int i=0; i<10; i++)
    {
		glNewList(digitsBase_+i, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, digits_[i].getID());
		glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(32.0f,-32.0f,10.0f);
			glTexCoord2f(0.0f,1.0f);glVertex3f(0.0f, 64.0f, 0.0f);
			glTexCoord2f(0.0f,0.0f);glVertex3f(0.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f,1.0f);glVertex3f(64.0f, 64.0f, 0.0f);
			glTexCoord2f(1.0f,0.0f);glVertex3f(64.0f, 0.0f, 0.0f);
		glEnd();
		glEndList();
	}
}
void FbgRenderer::killLists()
{
	glDeleteLists(cube_, 2);
	glDeleteLists(digitsBase_, 10);
}
void FbgRenderer::drawDigits(const string & str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] >= 48 && str[i] <= 57)
			glCallList(digitsBase_ + str[i] - 48);
		glTranslatef(64.0f, 0.0f, 0.0f);
	}
	// Move back
	glTranslatef(-float (str.size() * 64), 0.0f, 0.0f);
}

void FbgRenderer::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Makes the visible screen 0-1024x 0-(-768)y
	glLoadIdentity();
	glTranslatef(-512.0f,384.0f,-927.05801f);

    vector<FbgGame *>& vec = GameMgr::Instance().GetGameVec();
    for(int i = 0; i < (int)vec.size(); ++i)
    {
        FbgGame *game = vec[i];
        string mode = game->getMode();
        int singleMode = ConfigMgr().GetIntegerValue(mode + ".single_mode");

        if(!singleMode)
        {
            glPushMatrix();
            glTranslatef(game->getDestCoord().first, game->getDestCoord().second, 0.0f);
            glScalef(0.5f, 0.5f, 0.5f);
        }

        drawBkgnd(game);
        drawCurBlockFrame(game);

        if(singleMode)
        {
            drawNextBlockFrame(game);
            drawNextBlock(game);
            drawGameInfo(game);
        }

	    if (game->getLineGain()) 
            drawLineGain(game);
	    else
        {
		    if (game->getState() == GAMEOVER)
                drawGameOver(game);
		    else
                drawGameplay(game);
	    }

        if(!singleMode)
        {
            drawGameID(game);
            glPopMatrix();

            glPushMatrix();
            glScalef(0.5f, 0.5f, 0.5f);
            drawMulGameInfo(game);
            glPopMatrix();
        }
    }

    SDL_GL_SwapBuffers();
}

void FbgRenderer::drawGameMatrix( FbgGame *game )
{
	glPushMatrix();
	glTranslatef(24.0f, -24.0f, -24.0f);
	for (int row = 0; row < 18; row++)
	{
		for (int col = 0; col < 10; col++)
		{
			if (game->getMatrixAt(row, col))
			{
				glBindTexture(GL_TEXTURE_2D, blocks_[game->getMatrixAt(row, col) - 1].getID());
				glCallList(cubeWithoutBack_);
			}
			glTranslatef(40.0f, 0.0f, 0.0f);
		}
		glTranslatef(-400.0f, -40.0f, 0.0f);	// Move to front of row
	}
	glPopMatrix();
}

void FbgRenderer::drawBlock( FbgGame *game, FbgBlock * theBlock )
{
	// Draw Next Block
	glPushMatrix();
	glTranslatef(48.0f + 627.0f, -568.0f, 0.0f);
	glTranslatef(float (theBlock->getWidth() % 2) * (20.0f),
		(4.0f - theBlock->getHeight()) / 2 * (-40.0f), 20.0f);
	glBindTexture(GL_TEXTURE_2D, blocks_[theBlock->getIndex()].getID());
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			if (theBlock->getMatrixAt(row, col))
				glCallList(cube_ + 1);
			glTranslatef(40.0f, 0.0f, 0.0f);
		}
		glTranslatef(-160.0f, -40.0f, 0.0f);	// Move to front of row
	}
	glPopMatrix();
}

void FbgRenderer::drawGameOver( FbgGame *game )
{
	glEnable(GL_LIGHTING);
	drawGameMatrix(game);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, gameover_[0].getID());
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, -256.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -512.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(256.0f, -256.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(256.0f, -512.0f, 0.0f);
		glEnd();
	glBindTexture(GL_TEXTURE_2D, gameover_[1].getID());
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(256.0f, -256.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(256.0f, -512.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(512.0f, -256.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(512.0f, -512.0f, 0.0f);
		glEnd();
}

void FbgRenderer::drawGameplay( FbgGame *game )
{
    glEnable(GL_LIGHTING);

	// Draw Matrix
	drawGameMatrix(game);

	// Current Block
	glPushMatrix();
	glTranslatef(24.0f + game->getCurBlock()->getPosX() * 40.0f,
		-24.0f - game->getCurBlock()->getPosY() * 40.0f , -24.0f);
	glBindTexture(GL_TEXTURE_2D, blocks_[game->getCurBlock()->getIndex()].getID());
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			if (game->getCurBlock()->getMatrixAt(row, col))
				glCallList(cube_ + 1);
			glTranslatef(40.0f, 0.0f, 0.0f);
		}
		glTranslatef(-160.0f, -40.0f, 0.0f);
	}
	glPopMatrix();

	glDisable(GL_LIGHTING);
}

void FbgRenderer::drawLineGain( FbgGame *game )
{
	// Draw non-transparent matrix
	glTranslatef(24.0f, -24.0f - 680.0f, -24.0f);
	for (int row = 17; row >= 0; row--)
	{
			for (int col = 0; col < 10; col++)
			{
				if (game->getMatrixAt(row ,  col))
				{
					glBindTexture(GL_TEXTURE_2D, blocks_[game->getMatrixAt( row , col) - 1].getID());
					glCallList(cubeWithoutBack_);
				}
				glTranslatef(40.0f, 0.0f, 0.0f);
			}
			glTranslatef(-400.0f, 40.0f, 0.0f);	// Move to front of row
	}
	glTranslatef(-24.0f, 24.0f - 40.0f, 24.0f);

	// If a level was gained
	if (game->getLineGain()->levelGained > 1)
	{
		char tmpString[3];
		glEnable(GL_BLEND);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f );
		glTranslatef(512.0f, -368.0f,  0.0f);
		sprintf(tmpString, "%02u", game->getLineGain()->levelGained);
		drawDigits(tmpString);
		glTranslatef(512.0f, -368.0f, 0.0f);
		glDisable(GL_BLEND);
	}

    drawCurBlock(game);
	
	glDisable(GL_LIGHTING);
}

void FbgRenderer::drawBkgnd(FbgGame *game)
{
    // ×ó²à±³¾°Í¼
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, bg_[0].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f);glVertex3f(0.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.90625f);glVertex3f(0.0f, -24.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);glVertex3f(256.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.90625f);glVertex3f(256.0f, -24.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[1].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(256.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.90625f); glVertex3f(256.0f, -24.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(512.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.90625f); glVertex3f(512.0f, -24.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[0].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.90625f); glVertex3f(0.0f, -24.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -256.0f, 0.0f);
        glTexCoord2f(0.09375f, 0.90625f); glVertex3f(24.0f, -24.0f, 0.0f);
        glTexCoord2f(0.09375f, 0.0f); glVertex3f(24.0f, -256.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[4].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 1); glVertex3f(0.0f, -256.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f(0.0f, -512.0f, 0.0f);
        glTexCoord2f(0.09375f, 1); glVertex3f(24.0f, -256.0f, 0.0f);
        glTexCoord2f(0.09375f, 0); glVertex3f(24.0f, -512.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[8].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 1); glVertex3f(0.0f, -512.0f, 0.0f);
        glTexCoord2f(0, 0.09375f); glVertex3f(0.0f, -744.0f, 0.0f);
        glTexCoord2f(0.09375f, 1); glVertex3f(24.0f, -512.0f, 0.0f);
        glTexCoord2f(0.09375f, 0.09375f); glVertex3f(24.0f, -744.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[1].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.65625f, 0.90625f); glVertex3f(424.0f, -24.0f, 0.0f);
        glTexCoord2f(0.65625f, 0.0f); glVertex3f(424.0f, -256.0f, 0.0f);
        glTexCoord2f(1.0f, 0.90625f); glVertex3f(512.0f, -24.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(512.0f, -256.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[5].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.65625f, 1.0f); glVertex3f(424.0f, -256.0f, 0.0f);
        glTexCoord2f(0.65625f, 0.0f); glVertex3f(424.0f, -512.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(512.0f, -256.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(512.0f, -512.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[9].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.65625f, 1.0f); glVertex3f(424.0f, -512.0f, 0.0f);
        glTexCoord2f(0.65625f, 0.09375f); glVertex3f(424.0f, -744.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(512.0f, -512.0f, 0.0f);
        glTexCoord2f(1.0f, 0.09375f); glVertex3f(512.0f, -744.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[8].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.09375f); glVertex3f(0.0f, -744.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -768.0f, 0.0f);
        glTexCoord2f(1.0f, 0.09375f); glVertex3f(256.0f, -744.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(256.0f, -768.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[9].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.09375f); glVertex3f(256.0f, -744.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(256.0f, -768.0f, 0.0f);
        glTexCoord2f(1.0f, 0.09375f); glVertex3f(512.0f, -744.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(512.0f, -768.0f, 0.0f);
    glEnd();

    ConfigMgr config;
    if( !config.GetIntegerValue(game->getMode() + ".single_mode") )
        return;

    // ÓÒ²à±³¾°Í¼
    glBindTexture(GL_TEXTURE_2D, bg_[2].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(512.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(512.0f, -256.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(768.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(768.0f, -256.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[3].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(768.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(768.0f, -256.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(1024.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1024.0f, -256.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[6].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(512.0f, -256.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(512.0f, -512.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(768.0f, -256.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(768.0f, -512.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[7].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(768.0f, -256.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(768.0f, -512.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(1024.0f, -256.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1024.0f, -512.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[10].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.09375f); glVertex3f(512.0f, -744.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(512.0f, -768.0f, 0.0f);
        glTexCoord2f(1.0f, 0.09375f); glVertex3f(768.0f, -744.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(768.0f, -768.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[11].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.09375f); glVertex3f(768.0f, -744.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(768.0f, -768.0f, 0.0f);
        glTexCoord2f(1.0f, 0.09375f); glVertex3f(1024.0f, -744.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1024.0f, -768.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[10].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(512.0f, -512.0f, 0.0f);
        glTexCoord2f(0.0f, 0.09375f); glVertex3f(512.0f, -744.0f, 0.0f);
        glTexCoord2f(0.5625f, 1.0f); glVertex3f(656.0f, -512.0f, 0.0f);
        glTexCoord2f(0.5625f, 0.09375f); glVertex3f(656.0f, -744.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[11].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.3125f, 1.0f); glVertex3f(848.0f, -512.0f, 0.0f);
        glTexCoord2f(0.3125f, 0.09375f); glVertex3f(848.0f, -744.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(1024.0f, -512.0f, 0.0f);
        glTexCoord2f(1.0f, 0.09375f); glVertex3f(1024.0f, -744.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[10].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.5625f, 1.0f); glVertex3f(656.0f, -512.0f, 0.0f);
        glTexCoord2f(0.5625f, 0.84375f); glVertex3f(656.0f, -552.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(768.0f, -512.0f, 0.0f);
        glTexCoord2f(1.0f, 0.84375f); glVertex3f(768.0f, -552.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[11].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(768.0f, -512.0f, 0.0f);
        glTexCoord2f(0.0f, 0.84375f); glVertex3f(768.0f, -552.0f, 0.0f);
        glTexCoord2f(0.3125f, 1.0f); glVertex3f(848.0f, -512.0f, 0.0f);
        glTexCoord2f(0.3125f, 0.84375f); glVertex3f(848.0f, -552.0f, 0.0f);
    glEnd();
}

void FbgRenderer::drawNextBlockFrame( FbgGame *game )
{
    // Next Block Insets
    glBindTexture(GL_TEXTURE_2D, nb_[0].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.5625f, 1.0f); glVertex3f(656.0f, -744.0f, -64.0f);
        glTexCoord2f(0.5625f, 0.0f); glVertex3f(656.0f, -744.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(768.0f, -744.0f, -64.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(768.0f, -744.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, nb_[1].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(768.0f, -744.0f, -64.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(768.0f, -744.0f, 0.0f);
        glTexCoord2f(0.3125f, 1.0f); glVertex3f(848.0f, -744.0f, -64.0f);
        glTexCoord2f(0.3125f, 0.0f); glVertex3f(848.0f, -744.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, nr_.getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.84375f); glVertex3f(848.0f, -552.0f, -64.0f);
        glTexCoord2f(0.0f, 0.09375f); glVertex3f(848.0f, -744.0f, -64.0f);
        glTexCoord2f(1.0f, 0.84375f); glVertex3f(848.0f, -552.0f, 0.0f);
        glTexCoord2f(1.0f, 0.09375f); glVertex3f(848.0f, -744.0f, 0.0f);
    glEnd();
    // Next Block Backing
    glBindTexture(GL_TEXTURE_2D, bg_[10].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.5625f, 0.84375f); glVertex3f(656.0f, -552.0f, -64.0f);
        glTexCoord2f(0.5625f, 0.125f); glVertex3f(656.0f, -744.0f, -64.0f);
        glTexCoord2f(1.0f, 0.84375f); glVertex3f(768.0f, -552.0f, -64.0f);
        glTexCoord2f(1.0f, 0.125f); glVertex3f(768.0f, -744.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[11].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.84375f); glVertex3f(768.0f, -552.0f, -64.0f);
        glTexCoord2f(0.0f, 0.125f); glVertex3f(768.0f, -744.0f, -64.0f);
        glTexCoord2f(0.3125f, 0.84375f); glVertex3f(848.0f, -552.0f, -64.0f);
        glTexCoord2f(0.3125f, 0.125f); glVertex3f(848.0f, -744.0f, -64.0f);
    glEnd();
}

void FbgRenderer::drawCurBlockFrame( FbgGame *game )
{
    // Main Insets
    glBindTexture(GL_TEXTURE_2D, ml_[0].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.90625f); glVertex3f(24.0f, -24.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(24.0f, -256.0f, 0.0f);
        glTexCoord2f(1.0f, 0.90625f); glVertex3f(24.0f, -24.0f, -64.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(24.0f, -256.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, ml_[1].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(24.0f, -256.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(24.0f, -512.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(24.0f, -256.0f, -64.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(24.0f, -512.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, ml_[2].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(24.0f, -512.0f, 0.0f);
        glTexCoord2f(0.0f, 0.09375f); glVertex3f(24.0f, -744.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(24.0f, -512.0f, -64.0f);
        glTexCoord2f(1.0f, 0.09375f); glVertex3f(24.0f, -744.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, mb_[0].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.09375f, 1.0f); glVertex3f(24.0f, -744.0f, -64.0f);
        glTexCoord2f(0.09375f, 0.0f); glVertex3f(24.0f, -744.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(256.0f, -744.0f, -64.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(256.0f, -744.0f, -0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, mb_[1].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(256.0f, -744.0f, -64.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(256.0f, -744.0f, 0.0f);
        glTexCoord2f(0.65625f, 1.0f); glVertex3f(424.0f, -744.0f, -64.0f);
        glTexCoord2f(0.65625f, 0.0f); glVertex3f(424.0f, -744.0f, -0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, mt_[0].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.09375f, 1.0f); glVertex3f(24.0f, -24.0f, 0.0f);
        glTexCoord2f(0.09375f, 0.0f); glVertex3f(24.0f, -24.0f, -64.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(256.0f, -24.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(256.0f, -24.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, mt_[1].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(256.0f, -24.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(256.0f, -24.0f, -64.0f);
        glTexCoord2f(0.65625f, 1.0f); glVertex3f(424.0f, -24.0f, 0.0f);
        glTexCoord2f(0.65625f, 0.0f); glVertex3f(424.0f, -24.0f, -64.0f);
    glEnd();

    // Main Backing
    glBindTexture(GL_TEXTURE_2D, bg_[0].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.09375f, 0.90625f); glVertex3f(24.0f, -24.0f, -64.0f);
        glTexCoord2f(0.09375f, 0.0f); glVertex3f(24.0f, -256.0f, -64.0f);
        glTexCoord2f(1.0f, 0.90625f); glVertex3f(256.0f, -24.0f, -64.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(256.0f, -256.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[1].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.90625f); glVertex3f(256.0f, -24.0f, -64.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(256.0f, -256.0f, -64.0f);
        glTexCoord2f(0.65625f, 0.90625f); glVertex3f(424.0f, -24.0f, -64.0f);
        glTexCoord2f(0.65625f, 0.0f); glVertex3f(424.0f, -256.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[4].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.09375f, 1.0f); glVertex3f(24.0f, -256.0f, -64.0f);
        glTexCoord2f(0.09375f, 0.0f); glVertex3f(24.0f, -512.0f, -64.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(256.0f, -256.0f, -64.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(256.0f, -512.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[5].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(256.0f, -256.0f, -64.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(256.0f, -512.0f, -64.0f);
        glTexCoord2f(0.65625f, 1.0f); glVertex3f(424.0f, -256.0f, -64.0f);
        glTexCoord2f(0.65625f, 0.0f); glVertex3f(424.0f, -512.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[8].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.09375f, 1.0f); glVertex3f(24.0f, -512.0f, -64.0f);
        glTexCoord2f(0.09375f, 0.09375f); glVertex3f(24.0f, -744.0f, -64.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(256.0f, -512.0f, -64.0f);
        glTexCoord2f(1.0f, 0.09375f); glVertex3f(256.0f, -744.0f, -64.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, bg_[9].getID());
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(256.0f, -512.0f, -64.0f);
        glTexCoord2f(0.0f, 0.09375f); glVertex3f(256.0f, -744.0f, -64.0f);
        glTexCoord2f(0.65625f, 1.0f); glVertex3f(424.0f, -512.0f, -64.0f);
        glTexCoord2f(0.65625f, 0.09375f); glVertex3f(424.0f, -744.0f, -64.0f);
    glEnd();
}

void FbgRenderer::drawGameInfo( FbgGame *game )
{
    // Draw Level, Lines, Score
    char tmpString[9];
    glPushMatrix();
        glTranslatef(512.0f, -368.0f, 0.0f);
        sprintf(tmpString, "%02u", game->getLevel());
        drawDigits(tmpString);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(800.0f, -368.0f, 0.0f);
        sprintf(tmpString, "%03u", game->getLines());
        drawDigits(tmpString);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(528.0f, -480.0f, 0.0f);
        sprintf(tmpString, "%07u", game->getScore());
        drawDigits(tmpString);
    glPopMatrix();
}

FbgRenderer::FbgRenderer()
{

}

FbgRenderer& FbgRenderer::Instance()
{
    return instance_;
}

void FbgRenderer::drawNextBlock( FbgGame * game )
{
    drawBlock(game, game->getBlockSet(game->getNextBlockIndex()));
}

void FbgRenderer::drawCurBlock( FbgGame * game )
{
    drawBlock(game, game->getCurBlock());
}

void FbgRenderer::drawMulGameInfo( FbgGame *game )
{
    glPushMatrix();
    float translateY = -256.0f * float(game->getID());
    glTranslatef(1024.0f, translateY, 0.0f);

    glBindTexture(GL_TEXTURE_2D, msb_.getID());
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(512.0f, -0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(512.0f, -256.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1024.0f, -0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1024.0f, -256.0f, 0.0f);
    glEnd();

    // Draw Level, Lines, Score
    char tmpString[9];
    glPushMatrix();
        glTranslatef(512.0f, -112.0f, 0.0f);
        sprintf(tmpString, "%02u", game->getLevel());
        drawDigits(tmpString);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(800.0f, -112.0f, 0.0f);
        sprintf(tmpString, "%u", game->getTotalLines());
        glTranslatef(64.0f*(3 - strlen(tmpString)), 0.0f, 0.0f);
        drawDigits(tmpString);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(528.0f, -244.0f, 0.0f);
        sprintf(tmpString, "%u", game->getTotalScore());
        glTranslatef(64.0f*(7 - strlen(tmpString)), 0.0f, 0.0f);
        drawDigits(tmpString);
    glPopMatrix();

    glPopMatrix();
}

void FbgRenderer::drawGameID( FbgGame *game )
{
    char tmpString[2];
    glPushMatrix();
        glTranslatef(0.0f, -64.0f, 0.0f);
        sprintf(tmpString, "%01u", game->getID());
        drawDigits(tmpString);
    glPopMatrix();
}
#pragma warning(default:4996)