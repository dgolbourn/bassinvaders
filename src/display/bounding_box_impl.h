#ifndef BOUNDING_BOX_IMPL_H_
#define BOUNDING_BOX_IMPL_H_

#include "SDL_rect.h"
#include "json.h"

namespace display
{
class BoundingBoxImpl
{
public:
  SDL_Rect rect_;

  BoundingBoxImpl(int x, int y, int w, int h);
  void Load(json::JSON const& json);
};
}
#endif