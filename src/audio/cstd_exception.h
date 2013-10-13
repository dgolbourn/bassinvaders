#ifndef CSTD_EXCEPTION_H_
#define CSTD_EXCEPTION_H_

#include <system_error>

namespace cstd
{
/**C standard library exception.  Throw this if a function from the C standard
library fails.*/
class Exception : public std::system_error
{
public:
  Exception() : std::system_error(errno, std::system_category()){}
};
}

#endif