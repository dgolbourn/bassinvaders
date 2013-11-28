#ifndef TTF_EXCEPTION_H_
#define TTF_EXCEPTION_H_

#include <string>
#include "SDL_ttf.h"

namespace ttf
{
class Exception : public std::runtime_error 
{
public:
  Exception() : std::runtime_error(TTF_GetError()) { }
};
}
#endif