#ifndef SCENE_H_
#define SCENE_H_
#include <memory>
#include <string>
#include "command.h"
#include "json.h"
#include "window.h"

namespace game
{
class Scene
{
public:
  Scene(void);
  Scene(std::string const& filename, display::Window& window);
  Scene(json::JSON const& json, display::Window& window);
  void Add(event::Command const& command, int z);
  void Render(void);

  ~Scene(void);
  Scene(Scene const& other);
  Scene(Scene&& other);
  Scene& operator=(Scene other);
private:
  std::shared_ptr<class SceneImpl> impl_;
};
}
#endif