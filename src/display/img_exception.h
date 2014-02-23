#ifndef IMG_EXCEPTION_H_
#define IMG_EXCEPTION_H_
#include "exception.h"
#include <string>
namespace img
{
std::string Error(void);

class Exception : virtual public exception::Exception
{
public:
  typedef boost::error_info<class What, std::string> What;
};
}
#endif