#ifndef RECT_H_
#define RECT_H_
#include "SDL_Rect.h"
namespace sdl
{
bool Intersection(SDL_Rect const* a, SDL_Rect const* b);
}
#endif