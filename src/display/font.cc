#include "font.h"
#include "font_impl.h"
#include "ttf_exception.h"

namespace display
{
FontImpl::FontImpl(std::string const& filename, int point, int r, int g, int b, int a) : ttf_(), colour_({r, g, b, a})
{
  font_ = TTF_OpenFont(filename.c_str(), point);
  if(!font_)
  {
    throw ttf::Exception();
  }
}

FontImpl::~FontImpl(void)
{
  TTF_CloseFont(font_);
}

Font::Font(std::string const& filename, int point, int r, int g, int b, int a)
{
  impl_ = std::make_shared<FontImpl>(filename, point, r, g, b, a);
}

Font::Font(void) noexcept
{
}
}