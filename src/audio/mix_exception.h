#ifndef MIX_EXCEPTION_H_
#define MIX_EXCEPTION_H_

#include <string>
#include "SDL_mixer.h"

namespace mix
{
class Exception : public std::runtime_error 
{
public:
  Exception() : std::runtime_error(Mix_GetError()) { }
};
}
#endif