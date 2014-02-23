#ifndef MIX_EXCEPTION_H_
#define MIX_EXCEPTION_H_
#include <string>
#include "exception.h"
namespace mix
{
std::string Error(void);

class Exception : virtual public exception::Exception
{
public:
  typedef boost::error_info<class What, std::string> What;
};
}
#endif