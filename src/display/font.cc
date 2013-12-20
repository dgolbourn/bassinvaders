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

bool Font::operator==(Font const& other) const
{
  return impl_ == other.impl_;
}

Font::Font(std::string const& filename, int point, int r, int g, int b, int a)
{
  impl_ = std::make_shared<FontImpl>(filename, point, r, g, b, a);
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