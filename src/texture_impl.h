#ifndef TEXTURE_IMPL_H_
#define TEXTURE_IMPL_H_

#include "SDL_rect.h"
#include "SDL_render.h"
#include "bounding_box.h"
#include "window_impl.h"

namespace display
{

class TextureImpl
{
public:
  TextureImpl(SDL_Texture* texture, WindowImpl* window);
  ~TextureImpl(void);

  void Free(void);
  void Render(void);
  void Render(BoundingBox& source, BoundingBox& destination);
  void Render(int x, int y);
  
  int reference_count_;
  SDL_Texture* texture_;
  WindowImpl* window_;
};

};
#endif