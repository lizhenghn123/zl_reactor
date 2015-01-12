#include "base/FileUtil.h"
#include <cstdio>
#include <fstream>
#include <string.h>
#ifdef OS_WINDOWS
#include <shlwapi.h>
#pragma comment(lib, "shlwapi")
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

NAMESPACE_ZL_START

bool isDirectory(const char *path)
{
#ifdef OS_WINDOWS
    return PathIsDirectoryA(path) ? true : false;
#else
    DIR *pdir = opendir(path);
    if(pdir != NULL)
    {
        closedir(pdir);
        pdir = NULL;
        return true;
    }
    return false;
#endif
}

void modifyDirPath(std::string& path) // 修改目录路径为X/Y/Z/
{
    if(path.empty())
    {
        return;
    }
    for(std::string::iterator iter = path.begin(); iter != path.end(); ++iter)
    {
        if(*iter == '\\')
        {
            *iter = '/';
        }
    }
    if(path.at(path.length() - 1) != '/')
    {
        path += "/";
    }
}

bool createRecursionDir(const char *dir)
{
    std::string dirs(dir);
    if(dirs.empty())
        return true;

    modifyDirPath(dirs);

    std::string::size_type pos = dirs.find('/');
    while(pos != std::string::npos)
    {
        std::string cur = dirs.substr(0, pos - 0);
        if(cur.length() > 0 && !isDirectory(cur.c_str()))
        {
            bool ret = false;
#ifdef OS_WINDOWS
            ret = CreateDirectoryA(cur.c_str(), NULL) ? true : false;
#else
            ret = (mkdir(cur.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0);
#endif
            if(!ret)
            {
                return false;
            }
        }
        pos = dirs.find('/', pos + 1);
    }

    return true;
}

bool isFileExist(const char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (!file)
        return false;

    ::fclose(file);
    return true;
    //std::ifstream infile(filepath);
    //return infile.good();
}

long getFileSize(FILE *file)
{
    if(file == NULL)
        return -1;
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    return fileSize;
}

long getFileSize(const char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if(file == NULL)
        return -1;
    long fileSize = getFileSize(file);
    fclose(file);
    return fileSize;
}

bool getFileData(const char *filepath, std::string& buf)
{
    FILE *file = fopen(filepath, "rb");
    if(file == NULL)
        return false;

    const static size_t PER_READ_SIZE = 1024;
    char data[PER_READ_SIZE];
    size_t size = PER_READ_SIZE;
    while(!feof(file) && size == PER_READ_SIZE)
    {
        ::memset(data, '\0', PER_READ_SIZE);
        size = fread(data, PER_READ_SIZE, 1, file);
        buf += data;
    }
    return true;
}


std::string getAppFullPath()
{
    const static size_t pathLen = 1024;
    char appFullPath[pathLen] = {0};

#ifdef OS_LINUX
    const static char *procExe = "/proc/self/exe";
    if(::readlink(procExe, appFullPath, pathLen) != -1)
        return appFullPath;
#else
    if (::GetModuleFileName(NULL, appFullPath, pathLen))
        return appFullPath;
#endif

    return "";
}

std::string getAppFileName()
{
    std::string path = getAppFullPath();
    if(path.empty())
        return path;

#ifdef OS_LINUX
    size_t pos = path.find_last_of("/");
    if(pos != std::string::npos)
        path = path.substr(pos + 1);
    return path;
#else
    size_t pos = path.find_last_of("\\/:");
    if (pos != std::string::npos)
        path = path.substr(pos + 1);
    pos = path.find_last_of('.');
    if (pos != std::string::npos)
        path = path.substr( 0, pos );
    return path;
#endif
}

NAMESPACE_ZL_END
