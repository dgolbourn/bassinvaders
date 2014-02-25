#include "rect.h"
#include <utility>
namespace sdl
{
static bool Intersection(float a_x, float a_w, float b_x, float b_w)
{
  float a_min = a_x;
  float a_max = a_x + a_w;
  if(a_min > a_max)
  {
    std::swap(a_min, a_max);
  }

  float b_min = b_x;
  float b_max = b_x + b_w;
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

bool Intersection(SDL_FRect const* a, SDL_FRect const* b)
{
  return Intersection(a->x, a->w, b->x, b->w) && Intersection(a->y, a->h, b->y, b->h);
}

void EnclosePoints(SDL_FPoint const* points, int count, SDL_FRect* result)
{
  float minx = points[0].x;
  float miny = points[0].y;
  float maxx = points[0].x;
  float maxy = points[0].y;

  for(int i = 1; i < count; ++i) 
  {
    float x = points[i].x;
    float y = points[i].y;
    if(x < minx) 
    {
      minx = x;
    } 
    else if(x > maxx) 
    {
      maxx = x;
    }
    if(y < miny) 
    {
      miny = y;
    } 
    else if(y > maxy) 
    {
      maxy = y;
    }
  }

  result->x = minx;
  result->y = miny;
  result->w = maxx - minx;
  result->h = maxy - miny;
}
}