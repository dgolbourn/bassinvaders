#ifndef WINDOW_IMPL_H_
#define WINDOW_IMPL_H_

#include <map>
#include <string>
#include <memory>
#include "SDL_video.h"
#include "SDL_render.h"
#include "texture.h"
#include "texture_impl.h"
#include "font.h"

namespace display
{

class WindowImpl
{
public:
  WindowImpl(std::string name);
  ~WindowImpl(void);

  Texture Load(std::string filename);
  Texture Text(std::string text, Font font);
  void Free(std::string filename);
  void Clear(void);
  void Show(void);

  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::map<std::string, Texture> files_;
};

}
#endif