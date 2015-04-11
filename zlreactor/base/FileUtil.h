// ***********************************************************************
// Filename         : FileUtil.h
// Author           : LIZHENG
// Created          : 2014-07-01
// Description      : 文件、目录相关工具函数
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-17
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_FILEUTIL_H
#define ZL_FILEUTIL_H
#include "Define.h"
#include <string>
NAMESPACE_ZL_START

namespace FileUtil
{
	string getBinaryPath();
	string getBinaryName();
	string getBinaryDir();

	bool   isDirectory(const char *dir);
	bool   createRecursionDir(const char *dir);

	bool   isFileExist(const char *filepath);
	long   getFileSize(FILE *file);
	long   getFileSize(const char *filepath);
	bool   readFile(const char *filepath, std::string& buf);
};

using namespace FileUtil;

NAMESPACE_ZL_END
#endif /* ZL_FILEUTIL_H */
