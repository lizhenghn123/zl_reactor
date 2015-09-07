#include "StringPiece.h"
#include <iostream>
//namespace zl { namespace base { 

std::ostream& operator<<(std::ostream& o, const zl::base::StringPiece& piece)
{
    return (o << piece.as_string());
}

//}  } // namespace zl { namespace base {

