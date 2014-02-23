#include "sdl_exception.h"
#include "SDL.h"
namespace sdl
{
std::string Error(void)
{
  return SDL_GetError();
}
}