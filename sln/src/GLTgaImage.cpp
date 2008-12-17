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

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <string.h>
#include <stdlib.h>
#include <physfs/physfs.h>
#include "glTGAImage.h"

GLTgaImage::~GLTgaImage()
{
    delete[] imageData;
}

bool GLTgaImage::loadTGAImage(PHYSFS_file * file)
{
    if (file == NULL)
        return 0;
    GLubyte loadedTGAHeader[12];
    GLubyte header[6];
    GLuint bytesPerPixel;
    GLuint imageSize;
    //GLuint tmp;
    GLuint type = GL_RGBA;

    if (file == NULL ||
        PHYSFS_read(file, loadedTGAHeader, 1, sizeof (loadedTGAHeader)) != sizeof (loadedTGAHeader)
        || memcmp(standardTGAHeader, loadedTGAHeader, sizeof (standardTGAHeader)) != 0
        || PHYSFS_read(file, header, 1, sizeof (header)) != sizeof (header))
    {
        if (file == NULL)
            return false;
        else
        {
            PHYSFS_close(file);
            return false;
        }
    }

    setWidth(header[1] * 256 + header[0]);
    setHeight(header[3] * 256 + header[2]);
    if (getWidth() <= 0 || getHeight() <= 0 || (header[4] != 24 && header[4] != 32))
    {
        PHYSFS_close(file);
        return false;
    }

    setBitsPerPixel(header[4]);
    bytesPerPixel = getBytesPerPixel();
    imageSize = getWidth() * getHeight() * bytesPerPixel;

    setImageData(new GLubyte[imageSize]);
    if (getImageData() == NULL || PHYSFS_read(file, getImageData(), 1, imageSize) != imageSize)
    {
        if (getImageData() != NULL)
            free(getImageData());
        PHYSFS_close(file);
        return false;
    }

    // Swamp BGR mode to RGB for OpenGL
    for (GLuint i = 0; i < imageSize; i += bytesPerPixel)
    {
        // 3 XOR's swap 2 bytes (or anything else, for that matter)
        getImageData(i)[0] ^= getImageData(i + 2)[0] ^= getImageData(i)[0] ^=
            getImageData(i + 2)[0];
    }

    PHYSFS_close(file);

    glGenTextures(1, getIDPointer());
    if (getBitsPerPixel() == 24)
        type = GL_RGB;

    glBindTexture(GL_TEXTURE_2D, getID());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, type, getWidth(), getHeight(), 0, type, GL_UNSIGNED_BYTE,
        getImageData());
    return true;
}

GLTgaImage *GLTgaImage::splitImageMap(int mapWidth, int mapHeight, int maxImages)
{
    GLuint imageSize;
    GLuint xImages, yImages;
    GLuint type = GL_RGBA;
    GLuint h;
    unsigned char bytesPerPixel = bpp / 8;

    // If the image won't cut up right, forget it
    if (width % mapWidth || height % mapHeight)
        return NULL;

    // maxImages=0 or more than possible returned images returns the max number of images
    xImages = width / mapWidth;
    yImages = height / mapHeight;
    if (!maxImages || maxImages > (int)xImages * (int)yImages)
        maxImages = (int)xImages * (int)yImages;
    GLTgaImage *target = new GLTgaImage[maxImages];
    for (h = 0; h < (GLuint)maxImages; h++)
    {
        target[h].setWidth(mapWidth);
        target[h].setHeight(mapHeight);
        target[h].setBitsPerPixel(bpp);
    }

    imageSize = mapWidth * mapHeight * bytesPerPixel;
    for (h = 0; h < (GLuint)maxImages; h++)
    {
        target[h].setImageData(new GLubyte[imageSize]);
    }

    // Remember OpenGL and TGA images are upside down!
    for (int row = yImages - 1; row >= 0; row--)
    {
        for (int line = 0; line < mapHeight; line++)
        {
            for (int col = 0; col < (int)xImages; col++)
            {
                memcpy(target[col + row * xImages].getImageData(line * mapWidth * bytesPerPixel),
                    &imageData[(yImages - 1 -
                            row) * xImages * mapHeight * mapWidth * bytesPerPixel +
                        line * mapWidth * xImages * bytesPerPixel + col * mapWidth * bytesPerPixel],
                    mapWidth * bytesPerPixel);
            }
        }
    }

    if (bpp == 24)
        type = GL_RGB;
    for (h = 0; h < (GLuint)maxImages; h++)
    {
        glGenTextures(1, target[h].getIDPointer());

        glBindTexture(GL_TEXTURE_2D, target[h].getID());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, type, mapWidth, mapHeight, 0, type, GL_UNSIGNED_BYTE,
            target[h].getImageData());
    }

    return target;
}

// Remember that these images are upside-down!
// FIXME: There's probably a better way to do this "cropping" function than what I'm doing right here... But it's really confusing!
bool GLTgaImage::cropImage(int newX, int newY, int newWidth, int newHeight)
{
    if (newWidth < 0 || newHeight < 0)
        return false;
    unsigned char Bpp = bpp / 8;

    GLubyte *newData = new GLubyte[newWidth * newHeight * Bpp];
    for (int y = newY; y < newHeight + newY; y++)
    {
        for (int x = newX; x < newWidth + newX; x++)
        {
            int tmpX = x;
            int tmpY = y;
            if (x >= (int)width)
                tmpX = width - 1;
            if (x < 0)
                tmpX = 0;
            if (y >= (int)height)
                tmpY = height - 1;
            if (y < 0)
                tmpY = 0;
            for (int q = 0; q < Bpp; q++)
                newData[(y - newY) * newWidth * Bpp + (x - newX) * Bpp + q] =
                    imageData[tmpY * width * Bpp + tmpX * Bpp + q];
        }
    }

    delete[] imageData;
    setImageData(newData);
    setWidth(newWidth);
    setHeight(newHeight);

    GLuint type = GL_RGBA;
    if (getBitsPerPixel() == 24)
        type = GL_RGB;

    glGenTextures(1, getIDPointer());

    glBindTexture(GL_TEXTURE_2D, getID());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, type, getWidth(), getHeight(), 0, type, GL_UNSIGNED_BYTE,
        getImageData());

    return true;
}

void GLTgaImage::makeClamped()
{
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
