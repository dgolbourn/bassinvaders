#ifndef IMG_EXCEPTION_H_
#define IMG_EXCEPTION_H_

#include "SDL_image.h"
#include <string>

namespace img
{
class Exception : public std::runtime_error 
{
public:
  Exception() : std::runtime_error(IMG_GetError()) { }
};
}
#endif