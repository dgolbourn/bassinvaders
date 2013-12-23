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
  HUD(std::string const& filename, display::Window& window, Scene& scene);
  HUD(json::JSON const& json, display::Window& window, Scene& scene);
  HUD(void);
  void Score(int score);
  void Life(int life);
  explicit operator bool(void) const;

  ~HUD(void);
  HUD(HUD const& other);
  HUD(HUD&& other);
  HUD& operator=(HUD other);
  typedef memory::WeakPtr<HUD, class HUDImpl> WeakPtr;
private:
  std::shared_ptr<class HUDImpl> impl_;
  friend WeakPtr;
};
}
#endif