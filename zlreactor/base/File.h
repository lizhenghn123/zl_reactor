/** \file IFile.h
 **	\date  2005-04-25
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2004-2011  Anders Hedstrom

This library is made available under the terms of the GNU GPL, with
the additional exemption that compiling, linking, and/or using OpenSSL 
is allowed.

If you would like to use this library in a closed-source application,
a separate license agreement is available. For information about 
the closed-source license agreement for the C++ sockets library,
please visit http://www.alhem.net/Sockets/license.html and/or
email license@alhem.net.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef ZL_FILE_H
#define ZL_FILE_H
#include "Define.h"
#include <stdio.h>
#include <string>
NAMESPACE_ZL_START

/** IFile : Pure virtual file I/O interface. */
class IFile
{
public:
    virtual ~IFile() {}

    virtual bool   fopen(const std::string& path, const std::string& mode) = 0;
    virtual void   fclose() const = 0;

    virtual size_t fread(char *, size_t, size_t) const = 0;
    virtual size_t fwrite(const char *, size_t, size_t) = 0;

    virtual char*  fgets(char *, int) const = 0;
    virtual void   fprintf(const char *format, ...) = 0;

    virtual off_t  size() const = 0;
    virtual bool   eof() const = 0;

    virtual void   reset_read() const = 0;
    virtual void   reset_write() = 0;

    virtual const std::string& path() const = 0;
};


/** File : implementation of a disk file. */
class File : public IFile
{
public:
    File();
    File(FILE *);
    File(const std::string& path, const std::string& mode);
    ~File();

    virtual bool   fopen(const std::string& path, const std::string& mode);
    virtual void   fclose() const;

    virtual size_t fread(char *, size_t, size_t) const;
    virtual size_t fwrite(const char *, size_t, size_t);

    virtual char*  fgets(char *, int) const;
    virtual void   fprintf(const char *format, ...);

    virtual off_t  size() const;
    virtual bool   eof() const;

    virtual void   reset_read() const;
    virtual void   reset_write();

    virtual const std::string& path() const;

private:
    File(const File& ) {} // copy constructor
    File& operator=(const File& ) { return *this; } // assignment operator

    std::string    m_path;
    std::string    m_mode;
    mutable FILE   *m_fil;
    bool           m_b_close;
    mutable long   m_rptr;
    long           m_wptr;
};


/** MemFile : implementation of a memory file. */
class MemFile : public IFile
{
    enum { BLOCKSIZE = 32768 };
public:
    /** File block structure. */
    struct block_t
    {
        block_t() : next(NULL) {}
        struct block_t *next;
        char data[BLOCKSIZE];
    };
public:
    /** Create temporary memory buffer, will be freed when object destructs */
    MemFile();
    /** Copy buffer from source, reset read ptr */
    MemFile(MemFile& );
    /** Read file and write to this, create non-temporary memory buffer from f.path() */
    MemFile(File& f);
    ~MemFile();

    virtual bool   fopen(const std::string& path, const std::string& mode);
    virtual void   fclose() const;

    virtual size_t fread(char* ptr, size_t size, size_t nmemb) const;
    virtual size_t fwrite(const char *ptr, size_t size, size_t nmemb);

    virtual char*  fgets(char *, int) const;
    virtual void   fprintf(const char *format, ...);

    virtual off_t  size() const;
    virtual bool   eof() const;

    virtual void   reset_read() const;
    virtual void   reset_write();

    virtual const std::string& path() const;

    /** Reference count when copy constructor is used */
    int     refCount() const;
    void    increase();
    void    decrease();

private:
    MemFile& operator=(const MemFile& ) { return *this; } // assignment operator

    MemFile&           m_src;
    bool               m_src_valid;
    block_t            *m_base;
    mutable block_t    *m_current_read;
    block_t            *m_current_write;
    int                m_current_write_nr;
    mutable size_t     m_read_ptr;
    size_t             m_write_ptr;
    mutable bool       m_b_read_caused_eof;
    int                m_ref_count;
    mutable bool       m_ref_decreased;
    std::string        m_path;
};

NAMESPACE_ZL_END
#endif  /* ZL_FILE_H */