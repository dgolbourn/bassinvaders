#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>
#include <memory>
#include <functional>
#include "font.h"
#include "bounding_box.h"
#include "json.h"

namespace display
{
typedef std::function<bool(display::BoundingBox const& source, display::BoundingBox const& destination, float parallax, bool tile, double angle)> Texture;

class Window
{
public:
  Window(json::JSON const& json);
  Window(void) = default;
  Texture Load(std::string const& filename);
  Texture Text(std::string const& text, Font const& font);
  void Free(void);
  void Clear(void) const;
  void Show(void) const;
  void View(int x, int y, float zoom);
private:
  std::shared_ptr<class WindowImpl> impl_;
};
}
#endif