#ifndef SDL_EXCEPTION_H_
#define SDL_EXCEPTION_H_

#include <string>
#include "SDL.h"

namespace sdl
{
/**SDL Error exception.  You can throw this if an SDL function returns an error
code.*/
class Exception : public std::runtime_error 
{
public:
  Exception() : std::runtime_error(SDL_GetError()) { }
};
};

#endif