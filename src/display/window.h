#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>
#include <memory>
#include "texture.h"
#include "font.h"
#include "bounding_box.h"

namespace display
{
class Window
{
public:
  Window(std::string const& name);
  Window(void);
  Texture Load(std::string const& filename);
  Texture Text(std::string const& text, Font const& font);
  void Free(std::string const& filename);
  void Free(void);
  void Clear(void) const;
  void Show(void) const;
  void View(int x, int y, float zoom);

  ~Window(void);
  Window(Window const& other);
  Window(Window&& other);
  Window& operator=(Window other);
private:
  std::shared_ptr<class WindowImpl> impl_;
};
}
#endif