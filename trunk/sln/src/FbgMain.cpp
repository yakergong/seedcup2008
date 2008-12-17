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

#include "FbgMusic.h"
#include "FbgResource.h"
#include "FbgRenderer.h"
#include "GameMgr.h"
#include "ConfigMgr.h"
#include <string>

using namespace std;

int main(int argc, char **argv)
{
    // 设置程序执行目录为当前工作目录
    ::SetCurrentDirectory( FbgResource::GetProgramDir().c_str() );

    ConfigMgr::Init("data/config.xml");
    ConfigMgr config;

    string mode = config.GetStringValue("option.mode");

    bool  fullscreen;
    if( config.GetIntegerValue(mode + ".full_screen") )
        fullscreen = true;
    else
        fullscreen = false;
    bool enableMusic;
    if( config.GetIntegerValue(mode + ".enable_music") )
        enableMusic = true;
    else
        enableMusic = false;
    int  width  = config.GetIntegerValue(mode + ".screen_width");
    int  height = config.GetIntegerValue(mode + ".screen_height");
    int  bpp = config.GetIntegerValue("global.bpp");;
    int  startLevel = 0;
    bool idleMode;
    if( config.GetIntegerValue(mode + ".idle_mode") )
        idleMode = true;
    else
        idleMode = false;
    string  prefix = "./";
    string  theme  = "";
    string  music  = "typea.it";

    // 创建游戏
    GameMgr::Instance().Init(idleMode, startLevel, mode);

    // 初始化数据
    FbgResource::Instance().Init(argv[0], prefix, theme);

    // 初始化音频
    if(enableMusic)
    {
        FbgMusic::Instance().initSound();
        FbgMusic::Instance().initMusic(music, &GameMgr::Instance());
    }

    // 初始化渲染器
    if ( !FbgRenderer::Instance().init(fullscreen, width, height, bpp) )
    {
        FbgRenderer::Instance().exit();
        exit(0);
    }                                    // Init Graphics

    // 游戏主循环
    FbgRenderer::Instance().draw();
    FbgRenderer::Instance().draw();            // Draw twice for double buffering
    GameMgr::Instance().Loop();

    // 关闭音乐
    if(enableMusic)
    {
        FbgMusic::Instance().stopMusic();
        FbgMusic::Instance().stopSound();
    }

    // 清理physfs
    FbgResource::Instance().Destroy();

    GameMgr::Instance().Destroy();

    return 0;
}
