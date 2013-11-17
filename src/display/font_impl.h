#ifndef FONT_IMPL_H_
#define FONT_IMPL_H_

#include <string>
#include "SDL_TTF.h"
#include "SDL_Pixels.h"
#include "ttf_manager.h"

namespace display
{
class FontImpl
{
public:
  FontImpl(std::string const& filename, int point, int r, int g, int b);
  
  ~FontImpl(void);

  TTF_Font* font_;
  SDL_Color colour_;
  ttf::Library const ttf_;
};
}
#endif