#include "hero.h"
#include "animation.h"
#include "bounding_box.h"
#include "alarm.h"

namespace game
{
class HeroImpl
{
public:
  HeroImpl(json::JSON const& json, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer& mixer);
  Animation moving_animation_;
  display::BoundingBox moving_render_box_;
  Alarm moving_sound_effect_;
  Animation destroyed_animation_;
  display::BoundingBox destroyed_render_box_;
  Alarm destroyed_sound_effect_;
  display::BoundingBox collision_box_;
  event::Command render_;
  event::Command pause_;
  bool paused_;
  Animation animation_;
  display::BoundingBox render_box_;
  Alarm sound_effect_;
};

class RenderCommand : public event::CommandImpl
{
public:
  RenderCommand(HeroImpl& impl);
  void operator()(void) final;
  HeroImpl& impl_;
};

RenderCommand::RenderCommand(HeroImpl& impl) : impl_(impl)
{
}

void RenderCommand::operator()(void)
{
  impl_.animation_.Render(impl_.render_box_);
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
    impl_.animation_.Resume();
    impl_.sound_effect_.Resume();
  }
  else
  {
    impl_.paused_ = true;
    impl_.animation_.Pause();
    impl_.sound_effect_.Pause();
  }
}

HeroImpl::HeroImpl(json::JSON const& json, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer& mixer)
{
  json_t* moving_animation;
  json_t* moving_render_box;
  json_t* moving_sound_effect;

  json_t* destroyed_animation;
  json_t* destroyed_render_box;
  json_t* destroyed_sound_effect;

  json_t* collision_box;

  json.Unpack("{s{sososo}s{sososo}so}", 0,
    "moving", "animation", &moving_animation, 
    "render box", &moving_render_box, 
    "sound effect", &moving_sound_effect,
    "destroyed", "animation", &destroyed_animation, 
    "render box", &destroyed_render_box, 
    "sound effect", &destroyed_sound_effect,
    "collision box", &collision_box);

  moving_animation_ = Animation(moving_animation, window);
  moving_render_box_ = display::BoundingBox(moving_render_box);
  moving_sound_effect_ = Alarm(moving_sound_effect, mixer);
    
  destroyed_animation_ = Animation(destroyed_animation, window);
  destroyed_render_box_ = display::BoundingBox(destroyed_render_box);
  destroyed_sound_effect_ = Alarm(destroyed_sound_effect, mixer);
  
  collision_box_ = display::BoundingBox(collision_box);
  render_ = event::Command(new RenderCommand(*this));
  scene.Add(render_, 0);
  paused_ = true;
  pause_ = event::Command(new PauseCommand(*this));
  pause.Add(pause_);

  animation_ = moving_animation_;
  render_box_ = moving_render_box_;
  sound_effect_ = moving_sound_effect_;
}

Hero::Hero(std::string const& filename, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer& mixer)
{
  impl_ = std::shared_ptr<HeroImpl>(new HeroImpl(json::JSON(filename), window, scene, collision, pause, mixer));
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer& mixer)
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