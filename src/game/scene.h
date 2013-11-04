#ifndef SCENE_H_
#define SCENE_H_

#include <memory>

namespace game
{

class RenderCommandImpl
{
public:
  virtual void operator()(void) = 0;
};

typedef std::shared_ptr<RenderCommandImpl> RenderCommand;

class Scene
{
public:
  Scene(void);
  void Subscribe(RenderCommand const& render, int z);
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