#pragma once

#include <physfs/physfs.h>
#include <string>

class FbgResource
{
public:
    static FbgResource& Instance();
    // 获取当前程序目录
    static std::wstring GetProgramDir();
    // PhysicsFS
    PHYSFS_file *LoadThemeFile(const std::string & file);
    bool Init(std::string argv0, std::string prefix_, std::string theme);
    void Destroy();

private:
    FbgResource();

    std::string theme_;
    std::string prefix_;
    std::string argv0_;
    static FbgResource instance_;
};