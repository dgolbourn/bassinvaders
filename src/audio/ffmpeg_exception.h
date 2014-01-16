#ifndef FFMPEG_EXCEPTION_H_
#define FFMPEG_EXCEPTION_H_

#include <stdexcept>
extern "C"
{
#include "libavutil/common.h"
#include "libavutil/error.h"
}

namespace ffmpeg
{
class Exception : public std::runtime_error 
{
public:
  Exception(int error = AVERROR_BUG);
};
}
#endif