#ifndef FFMPEG_EXCEPTION_H_
#define FFMPEG_EXCEPTION_H_

#include <stdexcept>

namespace ffmpeg
{
class Exception : public std::runtime_error 
{
public:
  Exception() : std::runtime_error("FFMPEG error") { }
};
}

#endif