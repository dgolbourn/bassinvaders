#include "window.h"
#include "window_impl.h"
#include <stdio.h>
#include <utility>
#include <cstdarg>
#include "sdl_manager.h"
#include "img_manager.h"
#include "ttf_manager.h"
#include "sdl_exception.h"
#include "img_exception.h"
#include "ttf_exception.h"
#include "texture_impl.h"
#include "font_impl.h"

WindowImpl::WindowImpl(void)
{
  try
  {
    sdl::init(SDL_INIT_VIDEO);
    img::init(IMG_INIT_PNG);
    ttf::init();
  }
  catch(...)
  {
    throw;
  }

	if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"))
	{
		fprintf(stderr, "Warning: SDL_SetHint() failed.");
	}

	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		fprintf(stderr, "Warning: SDL_SetHint() failed.");
	}

	window_ = SDL_CreateWindow("Bass Invaders", SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

	if(!window_)
	{
    throw sdl::Exception();
	}
	else
	{
		renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
		if(!renderer_)
		{
		  throw sdl::Exception();
		}
		else
		{
			SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
		}
	}
}

WindowImpl::~WindowImpl(void)
{
  for(auto iter = textures_.begin(); iter != textures_.end(); iter++) 
  {
    iter->first->window_ = nullptr;
  }

	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	
  ttf::quit();
  img::quit(IMG_INIT_PNG);
	sdl::quit(SDL_INIT_VIDEO);
}

Texture WindowImpl::Load(std::string filename)
{
  TextureImpl* texture;

	auto fileiter = files_.find(filename);
	if(fileiter != files_.end())
	{
    texture = fileiter->second;
    texture->reference_count_++;
	}
	else
	{
    SDL_Surface* surface = IMG_Load(filename.c_str());
		if(!surface)
		{
			throw img::Exception();
		}
    SDL_SetColorKey(surface, SDL_TRUE, 
      SDL_MapRGB(surface->format, 0, 255, 0));

    SDL_Texture* sdl_texture = SDL_CreateTextureFromSurface(renderer_, surface);
		if(!sdl_texture)
		{
			throw sdl::Exception();
		}
		SDL_FreeSurface(surface);

    texture = new TextureImpl(sdl_texture, this);

		files_[filename] = texture;
    textures_[texture] = filename;
	}

  Texture T;
  T.impl_ = texture;

  return T;
}

Texture WindowImpl::Text(std::string text, Font font)
{
  SDL_Surface* surface = TTF_RenderText_Solid(font.impl_->font_, text.c_str(),
    *font.impl_->colour_);
	if(!surface)
	{
		throw ttf::Exception();
	}

  SDL_Texture* sdl_texture = SDL_CreateTextureFromSurface(renderer_, surface);
	if(!sdl_texture)
	{
		throw sdl::Exception();
	}
	SDL_FreeSurface(surface);

  TextureImpl* texture = new TextureImpl(sdl_texture, this);
  textures_[texture] = "";

  Texture T;
  T.impl_ = texture;
  return T;
}

void WindowImpl::Clear(void)
{
  SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear(renderer_);
}

void WindowImpl::Show(void)
{
  SDL_RenderPresent(renderer_);
}

Window::Window(void)
{
  impl_ = new WindowImpl();
}

Window::~Window(void)
{
  delete impl_;
}

Texture Window::Load(std::string filename)
{
  return impl_->Load(filename);
}

void Window::Clear(void)
{
  return impl_->Clear();
}

void Window::Show(void)
{
  return impl_->Show();
}

Texture Window::Text(std::string text, Font font)
{
  return impl_->Text(text, font);
}