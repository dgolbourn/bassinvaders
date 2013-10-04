#ifndef WINDOW_IMPL_H_
#define WINDOW_IMPL_H_

#include <map>
#include <string>
#include "SDL_video.h"
#include "SDL_render.h"
#include "texture.h"
#include "texture_impl.h"
#include "font.h"

class WindowImpl
{
public:
  WindowImpl(void);
  ~WindowImpl(void);
  Texture Load(std::string filename);
  Texture Text(std::string text, Font font);
  void Clear(void);
  void Show(void);

  SDL_Window* window_;
  SDL_Renderer* renderer_;

  std::map<std::string, TextureImpl*> files_;
  std::map<TextureImpl*, std::string> textures_;
};

#endif