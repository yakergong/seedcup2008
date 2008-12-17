#include "FbgResource.h"
#include <Windows.h>
#include <mikmod/mikmod.h>

using namespace std;

// PhysicsFS-MREADER wrapper
struct physfsMREADER
{
    MREADER core;
    PHYSFS_file *file;
};

FbgResource FbgResource::instance_;

FbgResource::FbgResource()
{

}

FbgResource& FbgResource::Instance()
{
    return instance_;
}

PHYSFS_file *FbgResource::LoadThemeFile(const string & file)
{
    if (theme_ != "" && PHYSFS_exists((string("themes/") + theme_ + "/" + file).c_str()))
        return PHYSFS_openRead((string("themes/") + theme_ + "/" + file).c_str());
    else
        return PHYSFS_openRead((string("themes/default/") + file).c_str());
}

bool FbgResource::Init(string argv0, string prefix, string theme)
{
    argv0_  = argv0;
    prefix_ = prefix;
    theme_  = theme;

    // Init PhysicsFS
    PHYSFS_init(argv0_.c_str());
    PHYSFS_addToSearchPath((prefix_ + PHYSFS_getDirSeparator() + "data" +
        PHYSFS_getDirSeparator()).c_str(), 1);

    PHYSFS_addToSearchPath((string(PHYSFS_getBaseDir()) + "data\\").c_str(), 1);

    // Load in .pk3 files
    char  **list = PHYSFS_enumerateFiles("");
    for (int i = 0; list[i] != NULL; i++)
    {
        int     end;
        for (end = 0; list[i][end] != '\0'; end++);
        if (list[i][end - 1] == '3' && list[i][end - 2] == 'k' && list[i][end - 3] == 'p'
            && list[i][end - 4] == '.')
        {
            PHYSFS_addToSearchPath((string(PHYSFS_getRealDir(list[i])) + list[i]).c_str(), 1);
        }
    }
    PHYSFS_freeList(list);

    return true;
}

void FbgResource::Destroy()
{
    PHYSFS_deinit();
}

std::wstring FbgResource::GetProgramDir()
{
    const int bufLen = MAX_PATH;
    wchar_t *buffer = new wchar_t[bufLen];
    int retVal = ::GetModuleFileName(NULL, buffer, MAX_PATH);
    if (retVal <= MAX_PATH-1)
    {
        // Remove filename from path
        wchar_t *tszSlash = wcsrchr(buffer, wchar_t(L'\\'));
        if (tszSlash)
            *tszSlash = L'\0';
    }

    wstring wstr(buffer);
    delete[] buffer;

    return wstr;
}

static long PhysfsMREADER_tell(MREADER * context)
{
    return (long) (PHYSFS_tell(((physfsMREADER *) (context))->file));
}

static BOOL PhysfsMREADER_seek(MREADER * context, long offset, int whence)
{
    PHYSFS_uint64 start;
    switch (whence)
    {
    case SEEK_CUR:
        start = (PHYSFS_uint64) (PHYSFS_tell(((physfsMREADER *) (context))->file));
        break;
    case SEEK_END:
        start = (PHYSFS_uint64) (PHYSFS_fileLength(((physfsMREADER *) (context))->file));
        break;
    default:
        start = 0;
    }
    return PHYSFS_seek(((physfsMREADER *) (context))->file, start + offset);
}

static BOOL PhysfsMREADER_read(MREADER * context, void *ptr, size_t size)
{
    return (BOOL)PHYSFS_read(((physfsMREADER *) (context))->file, ptr, 1, (PHYSFS_uint32)size);
}

static int PhysfsMREADER_get(MREADER * context)
{
    PHYSFS_sint8 byte;
    PhysfsMREADER_read(context, &byte, 1);
    return byte;
}

static BOOL PhysfsMREADER_eof(MREADER * context)
{
    return PHYSFS_eof(((physfsMREADER *) (context))->file);
}

static physfsMREADER *getPhysfsMREADER(const string & theFile)
{
    physfsMREADER *reader = new physfsMREADER;
    reader->file = PHYSFS_openRead(theFile.c_str());
    reader->core.Seek = PhysfsMREADER_seek;
    reader->core.Tell = PhysfsMREADER_tell;
    reader->core.Read = PhysfsMREADER_read;
    reader->core.Get  = PhysfsMREADER_get;
    reader->core.Eof  = PhysfsMREADER_eof;

    return reader;
}

void freePhysfsMREADER(physfsMREADER * context)
{
    delete  context;
}