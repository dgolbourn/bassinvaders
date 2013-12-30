#include "rect.h"
#include <utility>
namespace sdl
{
static bool Intersection(int a_x, int a_w, int b_x, int b_w)
{
  int a_min = a_x;
  int a_max = a_x + a_w;
  if(a_min > a_max)
  {
    std::swap(a_min, a_max);
  }

  int b_min = b_x;
  int b_max = b_x + b_w;
  if(b_min > b_max)
  {
    std::swap(b_min, b_max);
  }

  if(b_min > a_min)
  {
    a_min = b_min;
  }
  if(b_max < a_max)
  {
    a_max = b_max;
  }
  bool intersection = true;
  if(a_max <= a_min)
  {
    intersection = false;
  }
  return intersection;
}

static bool Empty(SDL_Rect const *a)
{
  return !a || a->w == 0 || a->h == 0;
}

bool Intersection(SDL_Rect const* a, SDL_Rect const* b)
{
  return !Empty(a) && !Empty(b) && Intersection(a->x, a->w, b->x, b->w) && Intersection(a->y, a->h, b->y, b->h);
}
}