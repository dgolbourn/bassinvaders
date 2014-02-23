#ifndef CSTD_EXCEPTION_H_
#define CSTD_EXCEPTION_H_
#include <system_error>
namespace cstd
{
class Exception : public std::system_error
{
public:
  Exception() : std::system_error(errno, std::system_category()){}
};
}
#endif