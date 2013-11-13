#ifndef SCENE_H_
#define SCENE_H_
#include <memory>
#include "command.h"

namespace game
{
class Scene
{
public:
  Scene(void);
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