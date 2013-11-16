#include "font.h"
#include "font_impl.h"
#include "ttf_exception.h"
#include "ttf_manager.h"

namespace display
{
FontImpl::FontImpl(std::string const& filename, int point, int r, int g, int b) : colour_({r, g, b, 0})
{
  ttf::Init();
  font_ = TTF_OpenFont(filename.c_str(), point);
  if(!font_)
  {
    ttf::Quit();
    throw ttf::Exception();
  }
}

FontImpl::~FontImpl(void)
{
  TTF_CloseFont(font_);
  ttf::Quit();
}

Font::Font(std::string const& filename, int point, int r, int g, int b) : impl_(new FontImpl(filename, point, r, g, b))
{
}

Font::Font(void)
{
}

Font::Font(Font const& other) : impl_(other.impl_)
{
}

Font::Font(Font&& other) : impl_(std::move(other.impl_))
{
}

Font::~Font(void)
{
}

Font& Font::operator=(Font other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}