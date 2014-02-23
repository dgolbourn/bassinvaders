#ifndef FFMPEG_EXCEPTION_H_
#define FFMPEG_EXCEPTION_H_
#include "exception.h"
#include <string>
namespace ffmpeg
{
std::string Error(int error);

class Exception : virtual public exception::Exception
{
public:
  typedef boost::error_info<class What, std::string> What;
};
}
#endif