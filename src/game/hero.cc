#include "hero.h"
#include "animation.h"
#include "bounding_box.h"

namespace game
{
class HeroImpl
{
public:
  HeroImpl(std::string const& filename, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer);
  HeroImpl(json::JSON const& json, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer);
  void Load(json::JSON const& json, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer);

  Animation moving_;
  display::BoundingBox moving_render_box_;
  Animation destroyed_;
  display::BoundingBox destroyed_render_box_;
  display::BoundingBox collision_box_;
  event::Command render_;
  event::Command pause_;
  bool paused_;
};

class RenderCommand : public event::CommandImpl
{
public:
  RenderCommand(Animation const& animation, display::BoundingBox const& bounding_box);
  void operator()(void) final;
  Animation animation_;
  display::BoundingBox bounding_box_;
};

RenderCommand::RenderCommand(Animation const& animation, display::BoundingBox const& bounding_box) :
  animation_(animation), bounding_box_(bounding_box)
{
}

void RenderCommand::operator()(void)
{
  animation_.Render(bounding_box_);
}

class PauseCommand : public event::CommandImpl
{
public:
  PauseCommand(HeroImpl& impl);
  void operator()(void) final;
  HeroImpl& impl_;
};

PauseCommand::PauseCommand(HeroImpl& impl) : impl_(impl)
{
}

void PauseCommand::operator()(void)
{
  if(impl_.paused_)
  {
    impl_.paused_ = false;
    static_cast<RenderCommand*>(impl_.render_.get())->animation_.Resume();
  }
  else
  {
    impl_.paused_ = true;
    static_cast<RenderCommand*>(impl_.render_.get())->animation_.Pause();
  }
}

HeroImpl::HeroImpl(std::string const& filename, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer)
{
  Load(json::JSON(filename), window, scene, collision, pause, mixer);
}

HeroImpl::HeroImpl(json::JSON const& json, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer)
{
  Load(json, window, scene, collision, pause, mixer);
}

void HeroImpl::Load(json::JSON const& json, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer)
{
  json_t* moving;
  json_t* moving_render_box;
  char const* ms;
  int mi;
  json_t* destroyed;
  json_t* destroyed_render_box;
  char const* ds;
  int di;
  json_t* collision_box;

  json.Unpack("{s{sosos{sssi}}s{sosos{sssi}}so}", 0,
    "moving", "animation", &moving, "render box", &moving_render_box, "sound effect", "file", &ms, "repeat interval", &mi,
    "destroyed", "animation", &destroyed, "render box", &destroyed_render_box, "sound effect", "file", &ds, "repeat interval", &di,
    "collision box", &collision_box);

  moving_ = Animation(moving, window);
  moving_render_box_ = display::BoundingBox(moving_render_box);
  destroyed_ = Animation(destroyed, window);
  destroyed_render_box_ = display::BoundingBox(destroyed_render_box);
  collision_box_ = display::BoundingBox(collision_box);
  render_ = event::Command(new RenderCommand(moving_, moving_render_box_));
  scene.Add(render_, 0);
  paused_ = true;
  pause_ = event::Command(new PauseCommand(*this));
  pause.Add(pause_);
}

Hero::Hero(std::string const& filename, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer)
{
  impl_ = std::shared_ptr<HeroImpl>(new HeroImpl(filename, window, scene, collision, pause, mixer));
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer)
{
  impl_ = std::shared_ptr<HeroImpl>(new HeroImpl(json, window, scene, collision, pause, mixer));
}

Hero::Hero(Hero const& other) : impl_(other.impl_)
{
}

Hero::Hero(Hero&& other) : impl_(std::move(other.impl_))
{
}

Hero::~Hero(void)
{
}

Hero& Hero::operator=(Hero other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}