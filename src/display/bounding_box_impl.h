#ifndef BOUNDING_BOX_IMPL_H_
#define BOUNDING_BOX_IMPL_H_

#include "SDL_rect.h"
#include "json.h"

namespace display
{
class BoundingBoxImpl
{
public:
  BoundingBoxImpl(int x, int y, int w, int h);
  BoundingBoxImpl(json::JSON const& json);
  SDL_Rect rect_;
};
}
#endif