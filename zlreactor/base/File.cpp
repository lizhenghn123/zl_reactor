#include "base/File.h"
#include <memory.h>
#include <stdarg.h>
#include "base/FileUtil.h"
NAMESPACE_ZL_START

File::File()
    :m_fil(NULL)
    ,m_b_close(true)
    ,m_rptr(0)
    ,m_wptr(0)
{
}

File::File(FILE *fil)
    :m_fil(fil)
    ,m_b_close(false)
    ,m_rptr(0)
    ,m_wptr(0)
{
}

File::File(const std::string& path, const std::string& mode)
    :m_fil(NULL)
    ,m_b_close(true)
    ,m_rptr(0)
    ,m_wptr(0)
{
    fopen(path, mode);
}

File::~File()
{
    if (m_b_close)
    {
        fclose();
    }
}

bool File::fopen(const std::string& path, const std::string& mode)
{
    m_path = path;
    m_mode = mode;

    m_fil = ::fopen(path.c_str(), mode.c_str());
    return m_fil ? true : false;
}

void File::fclose() const
{
    if (m_fil)
    {
        ::fclose(m_fil);
        m_fil = NULL;
    }
}

size_t File::fread(char *ptr, size_t size, size_t nmemb) const
{
    size_t r = 0;
    if (m_fil)
    {
        fseek(m_fil, m_rptr, SEEK_SET);
        r = ::fread(ptr, size, nmemb, m_fil);
        m_rptr = ftell(m_fil);
    }
    return r;
}

size_t File::fwrite(const char *ptr, size_t size, size_t nmemb)
{
    size_t r = 0;
    if (m_fil)
    {
        fseek(m_fil, m_wptr, SEEK_SET);
        r = ::fwrite(ptr, size, nmemb, m_fil);
        m_wptr = ftell(m_fil);
    }
    return r;
}

char *File::fgets(char *s, int size) const
{
    char *r = NULL;
    if (m_fil)
    {
        fseek(m_fil, m_rptr, SEEK_SET);
        r = ::fgets(s, size, m_fil);
        m_rptr = ftell(m_fil);
    }
    return r;
}

void File::fprintf(const char *format, ...)
{
    if (!m_fil)
        return;
    va_list ap;
    va_start(ap, format);
    fseek(m_fil, m_wptr, SEEK_SET);
    vfprintf(m_fil, format, ap);
    m_wptr = ftell(m_fil);
    va_end(ap);
}

off_t File::size() const
{
    return FileUtil::getFileSize(m_path.c_str());
}

bool File::eof() const
{
    if (m_fil)
    {
        if (feof(m_fil))
            return true;
    }
    return false;
}

void File::reset_read() const
{
    m_rptr = 0;
}

void File::reset_write()
{
    m_wptr = 0;
}

const std::string& File::path() const
{
    return m_path;
}

//--------------------------- MemFile ------------------------
MemFile::MemFile()
    :m_src(m_src)
    ,m_src_valid(false)
    ,m_base(new block_t)
    ,m_current_read(m_base)
    ,m_current_write(m_base)
    ,m_current_write_nr(0)
    ,m_read_ptr(0)
    ,m_write_ptr(0)
    ,m_b_read_caused_eof(false)
    ,m_ref_count(0)
    ,m_ref_decreased(false)
{
}

MemFile::MemFile(MemFile& s)
    :m_src(s)
    ,m_src_valid(true)
    ,m_base(s.m_base)
    ,m_current_read(m_base)
    ,m_current_write(s.m_current_write)
    ,m_current_write_nr(s.m_current_write_nr)
    ,m_read_ptr(0)
    ,m_write_ptr(s.m_write_ptr)
    ,m_b_read_caused_eof(false)
    ,m_ref_count(0)
    ,m_ref_decreased(false)
    ,m_path(s.m_path)
{
    m_src.increase();
}

MemFile::MemFile(File& f)
    :m_src(m_src)
    ,m_src_valid(false)
    ,m_base(new block_t)
    ,m_current_read(NULL)
    ,m_current_write(NULL)
    ,m_current_write_nr(0)
    ,m_read_ptr(0)
    ,m_write_ptr(0)
    ,m_b_read_caused_eof(false)
    ,m_ref_count(0)
    ,m_ref_decreased(false)
    ,m_path(f.path())
{
    m_current_read = m_base;
    m_current_write = m_base;
    char slask[32768];
    size_t n;
    while ((n = f.fread(slask, 1, 32768)) > 0)
    {
        fwrite(slask, 1, n);
    }
}

MemFile::~MemFile()
{
    if (m_ref_count)
    {
        std::cerr << "MemFile destructor with a ref count of " << m_ref_count << std::endl;
    }
    while (m_base && !m_src_valid)
    {
        block_t *p = m_base;
        m_base = p -> next;
        delete p;
    }
    if (m_src_valid && !m_ref_decreased)
    {
        m_src.decrease();
        m_ref_decreased = true;
    }
}

bool MemFile::fopen(const std::string& path, const std::string& mode)
{
    return true;
}

void MemFile::fclose() const
{
    if (m_src_valid && !m_ref_decreased)
    {
        m_src.decrease();
        m_ref_decreased = true;
    }
}

size_t MemFile::fread(char *ptr, size_t size, size_t nmemb) const
{
    size_t p = m_read_ptr % BLOCKSIZE;
    size_t sz = size * nmemb;
    size_t available = m_write_ptr - m_read_ptr;
    if (sz > available) // read beyond eof
    {
        sz = available;
        m_b_read_caused_eof = true;
    }
    else if (p + sz < BLOCKSIZE)
    {
        memcpy(ptr, m_current_read -> data + p, sz);
        m_read_ptr += sz;
    }
    else
    {
        size_t sz1 = BLOCKSIZE - p;
        size_t sz2 = sz - sz1;
        memcpy(ptr, m_current_read -> data + p, sz1);
        m_read_ptr += sz1;
        while (sz2 > BLOCKSIZE)
        {
            if (m_current_read->next)
            {
                m_current_read = m_current_read->next;
                memcpy(ptr + sz1, m_current_read->data, BLOCKSIZE);
                m_read_ptr += BLOCKSIZE;
                sz1 += BLOCKSIZE;
                sz2 -= BLOCKSIZE;
            }
            else
            {
                return sz1;
            }
        }
        if (m_current_read->next)
        {
            m_current_read = m_current_read->next;
            memcpy(ptr + sz1, m_current_read->data, sz2);
            m_read_ptr += sz2;
        }
        else
        {
            return sz1;
        }
    }
    return sz;
}

size_t MemFile::fwrite(const char *ptr, size_t size, size_t nmemb)
{
    size_t p = m_write_ptr % BLOCKSIZE;
    int nr = (int)m_write_ptr / BLOCKSIZE;
    size_t sz = size * nmemb;
    if (m_current_write_nr < nr)
    {
        block_t *next = new block_t;
        m_current_write -> next = next;
        m_current_write = next;
        m_current_write_nr++;
    }
    if (p + sz <= BLOCKSIZE)
    {
        memcpy(m_current_write -> data + p, ptr, sz);
        m_write_ptr += sz;
    }
    else
    {
        size_t sz1 = BLOCKSIZE - p; // size left
        size_t sz2 = sz - sz1;
        memcpy(m_current_write -> data + p, ptr, sz1);
        m_write_ptr += sz1;
        while (sz2 > BLOCKSIZE)
        {
            if (m_current_write -> next)
            {
                m_current_write = m_current_write -> next;
                m_current_write_nr++;
            }
            else
            {
                block_t *next = new block_t;
                m_current_write -> next = next;
                m_current_write = next;
                m_current_write_nr++;
            }
            memcpy(m_current_write -> data, ptr + sz1, BLOCKSIZE);
            m_write_ptr += BLOCKSIZE;
            sz1 += BLOCKSIZE;
            sz2 -= BLOCKSIZE;
        }
        if (m_current_write -> next)
        {
            m_current_write = m_current_write -> next;
            m_current_write_nr++;
        }
        else
        {
            block_t *next = new block_t;
            m_current_write -> next = next;
            m_current_write = next;
            m_current_write_nr++;
        }
        memcpy(m_current_write -> data, ptr + sz1, sz2);
        m_write_ptr += sz2;
    }
    return sz;
}

char *MemFile::fgets(char *s, int size) const
{
    int n = 0;
    while (n < size - 1 && !eof())
    {
        char c;
        size_t sz = fread(&c, 1, 1);
        if (sz)
        {
            if (c == 10)
            {
                s[n] = 0;
                return s;
            }
            s[n++] = c;
        }
    }
    s[n] = 0;
    return s;
}

void MemFile::fprintf(const char *format, ...)
{
    va_list ap;
    char tmp[BLOCKSIZE];
    va_start(ap, format);
    vsnprintf(tmp, sizeof(tmp), format, ap);
    va_end(ap);
    fwrite(tmp, 1, strlen(tmp));
}

off_t MemFile::size() const
{
    return (off_t)m_write_ptr;
}

bool MemFile::eof() const
{
    return m_b_read_caused_eof; //(m_read_ptr < m_write_ptr) ? false : true;
}

void MemFile::reset_read() const
{
    m_read_ptr = 0;
    m_current_read = m_base;
}

void MemFile::reset_write()
{
    m_write_ptr = 0;
    m_current_write = m_base;
    m_current_write_nr = 0;
}

const std::string& MemFile::path() const
{
    return m_path;
}

int MemFile::refCount() const
{
    return m_ref_count;
}

void MemFile::increase()
{
    ++m_ref_count;
}

void MemFile::decrease()
{
    --m_ref_count;
}

NAMESPACE_ZL_END