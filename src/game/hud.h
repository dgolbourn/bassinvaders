#ifndef HUD_H_
#define HUD_H_
#include <memory>
#include <string>
#include "json.h"
#include "window.h"
#include "scene.h"
#include "weak_ptr.h"

namespace game
{
class HUD
{
public:
  HUD(json::JSON const& json, display::Window& window, Scene& scene);
  HUD(void) = default;
  void Score(int score);
  void Life(int life);
  explicit operator bool(void) const;
  typedef memory::WeakPtr<HUD, class HUDImpl> WeakPtr;
private:
  std::shared_ptr<class HUDImpl> impl_;
  friend WeakPtr;
};
}
#endif