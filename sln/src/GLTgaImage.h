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

#ifndef GLTGAIMAGE_H
#define GLTGAIMAGE_H

#include <windows.h>
#include <GL/gl.h>
#include <physfs/physfs.h>

static GLubyte standardTGAHeader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

class   GLTgaImage
{
  public:
    bool loadTGAImage(PHYSFS_file * file);
    GLTgaImage *splitImageMap(int mapWidth, int mapHeight, int maxImages = 0);
    bool    cropImage(int newX, int newY, int newWidth, int newHeight);
    void    makeClamped();

    // Accessors
    GLubyte *getImageData()
    {
        return imageData;
    }
    GLubyte *getImageData(GLuint seek)
    {
        return &imageData[seek];
    }
    GLuint  getBitsPerPixel()
    {
        return bpp;
    }
    GLuint  getBytesPerPixel()
    {
        return bpp / 8;
    }
    GLuint  getWidth()
    {
        return width;
    }
    GLuint  getHeight()
    {
        return height;
    }
    GLuint  getID()
    {
        return texID;
    }
    GLuint *getIDPointer()
    {
        return &texID;
    }

    void    setImageData(GLubyte * newVar)
    {
        imageData = newVar;
    }
    void    setImageData(GLubyte * newVar, GLuint seek)
    {
        imageData[seek] = newVar[0];
    }
    void    setBitsPerPixel(GLuint newVar)
    {
        bpp = newVar;
    }
    void    setBytesPerPixel(GLuint newVar)
    {
        bpp = newVar * 8;
    }
    void    setWidth(GLuint newVar)
    {
        width = newVar;
    }
    void    setHeight(GLuint newVar)
    {
        height = newVar;
    }

    // Initialization
    ~GLTgaImage();
  private:
    GLubyte * imageData;
    GLuint  bpp;
    GLuint  width;
    GLuint  height;
    GLuint  texID;
};

#endif
