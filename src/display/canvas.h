#ifndef CANVAS_H_
#define CANVAS_H_

#include <memory>
#include "SDL_render.h"

namespace sdl
{
class Canvas
{
public:
  Canvas(void);
  Canvas(SDL_Renderer* renderer, SDL_Window* window, int w, int h);
  void Lock(void);
  void Unlock(void);
  void Render(SDL_Rect const* destination, double angle);

  Canvas(Canvas const& other);
  Canvas(Canvas&& other);
  ~Canvas(void);
  Canvas& operator=(Canvas other);
private:
  std::shared_ptr<class CanvasImpl> impl_;
};
}
#endif