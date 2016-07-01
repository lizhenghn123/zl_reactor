#include "zlreactor/base/StringPiece.h"
#include <limits.h>     // for UCHAR_MAX
#include <iostream>
#include <algorithm>
namespace zl { namespace base { 

typedef StringPiece::size_type size_type;

size_type StringPiece::find(const StringPiece& s, size_type pos) const
{
    if (pos > length_)
        return npos;

    const char* result = std::search(ptr_ + pos, ptr_ + length_,
        s.ptr_, s.ptr_ + s.length_);
    const size_type xpos = result - ptr_;
    return xpos + s.length_ <= length_ ? xpos : npos;
}

size_type StringPiece::find(char c, size_type pos) const
{
    if (pos >= length_)
        return npos;

    const void* result = memchr(ptr_ + pos, c, length_ - pos);
    if (result)
        return reinterpret_cast<const char*>(result)-ptr_;
    return npos;
}

size_type StringPiece::rfind(const StringPiece& s, size_type pos) const
{
    if (length_ < s.length_)
        return npos;

    if (s.empty())
        return std::min(length_, pos);

    const char* last = ptr_ + std::min(length_ - s.length_, pos) + s.length_;
    const char* result = std::find_end(ptr_, last, s.ptr_, s.ptr_ + s.length_);
    return result != last ? static_cast<size_t>(result - ptr_) : npos;
}

size_type StringPiece::rfind(char c, size_type pos) const
{
    if (length_ == 0)
        return npos;

    for (size_type i = std::min(pos, length_ - 1);; --i)
    {
        if (ptr_[i] == c)
            return i;
        if (i == 0)
            break;
    }
    return npos;
}

// For each character in characters_wanted, sets the index corresponding
// to the ASCII code of that character to 1 in table.  This is used by
// the m_find.*_of methods below to tell whether or not a character is in
// the lookup table in constant time.
// The argument `table' must be an array that is large enough to hold all
// the possible values of an unsigned char.  Thus it should be be declared
// as follows:
//   bool table[UCHAR_MAX + 1]
static inline void BuildLookupTable(const StringPiece& characters_wanted, bool* table)
{
    const size_type length = characters_wanted.length();
    const char* const data = characters_wanted.data();
    for (size_type i = 0; i < length; ++i)
    {
        table[static_cast<unsigned char>(data[i])] = true;
    }
}

size_type StringPiece::find_first_of(const StringPiece& s, size_type pos) const
{
    if (length_ == 0 || s.length_ == 0)
        return npos;

    // Avoid the cost of BuildLookupTable() for a single-character search.
    if (s.length_ == 1)
        return find_first_of(s.ptr_[0], pos);

    bool lookup[UCHAR_MAX + 1] = { false };
    BuildLookupTable(s, lookup);
    for (size_type i = pos; i < length_; ++i)
    {
        if (lookup[static_cast<unsigned char>(ptr_[i])])
        {
            return i;
        }
    }
    return npos;
}
size_type StringPiece::find_first_not_of(const StringPiece& s, size_type pos) const
{
    if (length_ == 0)
        return npos;

    if (s.length_ == 0)
        return 0;

    // Avoid the cost of BuildLookupTable() for a single-character search.
    if (s.length_ == 1)
        return find_first_not_of(s.ptr_[0], pos);

    bool lookup[UCHAR_MAX + 1] = { false };
    BuildLookupTable(s, lookup);
    for (size_type i = pos; i < length_; ++i)
    {
        if (!lookup[static_cast<unsigned char>(ptr_[i])])
        {
            return i;
        }
    }
    return npos;
}

size_type StringPiece::find_first_not_of(char c, size_type pos) const
{
    if (length_ == 0)
        return npos;

    for (; pos < length_; ++pos) {
        if (ptr_[pos] != c) {
            return pos;
        }
    }
    return npos;
}

size_type StringPiece::find_last_of(const StringPiece& s, size_type pos) const
{
    if (length_ == 0 || s.length_ == 0)
        return npos;

    // Avoid the cost of BuildLookupTable() for a single-character search.
    if (s.length_ == 1)
        return find_last_of(s.ptr_[0], pos);

    bool lookup[UCHAR_MAX + 1] = { false };
    BuildLookupTable(s, lookup);
    for (size_type i = std::min(pos, length_ - 1);; --i)
    {
        if (lookup[static_cast<unsigned char>(ptr_[i])])
            return i;
        if (i == 0)
            break;
    }
    return npos;
}

size_type StringPiece::find_last_not_of(const StringPiece& s, size_type pos) const {
    if (length_ == 0)
        return npos;

    size_type i = std::min(pos, length_ - 1);
    if (s.length_ == 0)
        return i;

    // Avoid the cost of BuildLookupTable() for a single-character search.
    if (s.length_ == 1)
        return find_last_not_of(s.ptr_[0], pos);

    bool lookup[UCHAR_MAX + 1] = { false };
    BuildLookupTable(s, lookup);
    for (;; --i)
    {
        if (!lookup[static_cast<unsigned char>(ptr_[i])])
            return i;
        if (i == 0)
            break;
    }
    return npos;
}

size_type StringPiece::find_last_not_of(char c, size_type pos) const
{
    if (length_ == 0)
        return npos;

    for (size_type i = std::min(pos, length_ - 1);; --i)
    {
        if (ptr_[i] != c)
            return i;
        if (i == 0)
            break;
    }
    return npos;
}

/* Like memcmp, but ignore differences in case.
Convert to upper case (not lower) before comparing so that
join -i works with sort -f.  */
int memcasecmp(const void *vs1, const void *vs2, size_t n)
{
    char const *s1 = static_cast<char const *>(vs1);
    char const *s2 = static_cast<char const *>(vs2);
    for (size_t i = 0; i < n; i++)
    {
        unsigned char u1 = s1[i];
        unsigned char u2 = s2[i];
        int U1 = toupper(u1);
        int U2 = toupper(u2);
        int diff = (UCHAR_MAX <= INT_MAX ? U1 - U2 : U1 < U2 ? -1 : U2 < U1);
        if (diff)
            return diff;
    }
    return 0;
}

int StringPiece::ignore_case_compare(const StringPiece& x) const
{
    int r = memcasecmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
    if (r != 0)
        return r;
    if (length_ < x.length_)
        return -1;
    else if (length_ > x.length_)
        return 1;
    return 0;
}

bool StringPiece::ignore_case_equal(const StringPiece& other) const
{
    return size() == other.size() && memcasecmp(data(), other.data(), size()) == 0;
}

}  } // namespace zl { namespace base {


std::ostream& operator<<(std::ostream& o, const zl::base::StringPiece& piece)
{
    return (o << piece.as_string());
}