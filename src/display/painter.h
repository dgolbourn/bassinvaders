#ifndef PAINTER_H_
#define PAINTER_H_
#include "SDL_render.h"
#include "flood_fill.h"
#include <memory>
namespace sdl
{
class Painter
{
public:
  Painter(void);
  Painter(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, SDL_Point const* view, double angle);
  bool operator()(algorithm::NodeCoordinates const& coords);

  Painter(Painter const& other);
  Painter(Painter&& other);
  Painter& operator=(Painter other);
  ~Painter(void);
private:
  std::shared_ptr<class PainterImpl> impl_;
};
}
#endif