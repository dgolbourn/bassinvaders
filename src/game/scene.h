#ifndef SCENE_H_
#define SCENE_H_
#include <memory>
#include <string>
#include "json.h"
#include "window.h"
#include "command.h"

namespace game
{
class Scene
{
public:
  Scene(void) noexcept;
  Scene(json::JSON const& json, display::Window& window);
  void Add(event::Command const& layer, int z);
  void Render(void);
private:
  std::shared_ptr<class SceneImpl> impl_;
};
}
#endif