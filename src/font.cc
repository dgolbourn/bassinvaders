#include "font.h"
#include "font_impl.h"
#include "ttf_exception.h"
#include "ttf_manager.h"

namespace display
{

FontImpl::FontImpl(std::string filename, int point, int r, int g, int b)
{
  ttf::init();
  font_ = TTF_OpenFont(filename.c_str(), point);
	if(!font_)
	{
		throw ttf::Exception();
	}
  reference_count_ = 1;

  colour_ = new SDL_Color;
  colour_->r = r;
  colour_->g = g;
  colour_->b = b;
  colour_->a = 0;
}

FontImpl::~FontImpl(void)
{
  delete colour_;
  TTF_CloseFont(font_);
  ttf::quit();
}

Font::Font(std::string filename, int point, int r, int g, int b)
{
  impl_ = new FontImpl(filename, point, r, g, b);
}

Font::Font(void)
{
  impl_ = nullptr;
}

Font::Font(const Font& original)
{
  impl_ = original.impl_;
  if(impl_)
  {
    impl_->reference_count_++;
  }
}

Font::Font(Font&& original)
{
  impl_ = original.impl_;
  original.impl_ = nullptr;
}

Font::~Font(void)
{
  if(impl_)
  {
    if(impl_->reference_count_ > 0)
    {
      impl_->reference_count_--;
    }
    if(impl_->reference_count_ == 0)
    {
      delete impl_;
      impl_ = nullptr;
    }
  }
}

Font& Font::operator=(Font original)
{
  std::swap(impl_, original.impl_);
  return *this;
}

};