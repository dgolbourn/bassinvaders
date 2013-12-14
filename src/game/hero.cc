#include "hero.h"
#include "animation.h"
#include "bounding_box.h"
#include "sound.h"
#include "event.h"
#include "dynamics.h"
#include <iostream>

namespace game
{
class HeroImpl
{
public:
  HeroImpl(json::JSON const& json, display::Window& window, Collision& collision, event::Signal& pause);
  void End(event::Command const& command);
  Animation moving_animation_;
  display::BoundingBox moving_render_box_;
  audio::Sound moving_sound_effect_;
  Animation destroyed_animation_;
  display::BoundingBox destroyed_render_box_;
  audio::Sound destroyed_sound_effect_;
  display::BoundingBox collision_box_;
  event::Command pause_;
  bool paused_;
  Animation animation_;
  display::BoundingBox render_box_;
  audio::Sound sound_effect_;
  event::Command attack_;
  event::Command up_;
  event::Command down_;
  event::Command left_;
  event::Command right_;
  event::Command enemy_collision_;
  class RenderCommand;
  class PauseCommand;
  event::Signal end_;
  Dynamics dynamics_;
  void Render(void);
};

void HeroImpl::End(event::Command const& command)
{
  end_.Add(command);
}

class HeroImpl::PauseCommand : public event::CommandImpl
{
public:
  PauseCommand(HeroImpl& impl);
  void operator()(void) final;
  HeroImpl& impl_;
};

HeroImpl::PauseCommand::PauseCommand(HeroImpl& impl) : impl_(impl)
{
}

void HeroImpl::PauseCommand::operator()(void)
{
  if(impl_.paused_)
  {
    impl_.paused_ = false;
    impl_.animation_.Resume();
    impl_.sound_effect_.Resume();
    impl_.dynamics_.Resume();
  }
  else
  {
    impl_.paused_ = true;
    impl_.animation_.Pause();
    impl_.sound_effect_.Pause();
    impl_.dynamics_.Pause();
  }
}

class AttackCommand : public event::CommandImpl
{
public:
  AttackCommand(HeroImpl& impl);
  void operator()(void) final;
  HeroImpl& impl_;
};

AttackCommand::AttackCommand(HeroImpl& impl) : impl_(impl)
{
}

void AttackCommand::operator()(void)
{
}

static float const dv = 0.1f;

class UpCommand : public event::CommandImpl
{
public:
  UpCommand(HeroImpl& impl);
  void operator()(void) final;
  HeroImpl& impl_;
  bool enabled_;
};

UpCommand::UpCommand(HeroImpl& impl) : impl_(impl), enabled_(true)
{
}

void UpCommand::operator()(void)
{
  if(enabled_)
  {
    impl_.dynamics_.v() -= dv;
    enabled_ = false;
  }
  else
  {
    impl_.dynamics_.v() += dv;
    enabled_ = true;
  }
}

class DownCommand : public event::CommandImpl
{
public:
  DownCommand(HeroImpl& impl);
  void operator()(void) final;
  HeroImpl& impl_;
  bool enabled_;
};

DownCommand::DownCommand(HeroImpl& impl) : impl_(impl), enabled_(true)
{
}

void DownCommand::operator()(void)
{
  if(enabled_)
  {
    impl_.dynamics_.v() += dv;
    enabled_ = false;
  }
  else
  {
    impl_.dynamics_.v() -= dv;
    enabled_ = true;
  }
}

class LeftCommand : public event::CommandImpl
{
public:
  LeftCommand(HeroImpl& impl);
  void operator()(void) final;
  HeroImpl& impl_;
  bool enabled_;
};

LeftCommand::LeftCommand(HeroImpl& impl) : impl_(impl), enabled_(true)
{
}

void LeftCommand::operator()(void)
{
  if(enabled_)
  {
    impl_.dynamics_.u() -= dv;
    enabled_ = false;
  }
  else
  {
    impl_.dynamics_.u() += dv;
    enabled_ = true;
  }
}

class RightCommand : public event::CommandImpl
{
public:
  RightCommand(HeroImpl& impl);
  void operator()(void) final;
  HeroImpl& impl_;
  bool enabled_;
};

RightCommand::RightCommand(HeroImpl& impl) : impl_(impl), enabled_(true)
{
}

void RightCommand::operator()(void)
{
  if(enabled_)
  {
    impl_.dynamics_.u() += dv;
    enabled_ = false;
  }
  else
  {
    impl_.dynamics_.u() -= dv;
    enabled_ = true;
  }
}

class EnemyCollisionCommand : public event::CommandImpl
{
public:
  EnemyCollisionCommand(HeroImpl& impl);
  void operator()(void) final;
  HeroImpl& impl_;
};

EnemyCollisionCommand::EnemyCollisionCommand(HeroImpl& impl) : impl_(impl)
{
}

void EnemyCollisionCommand::operator()(void)
{
  impl_.sound_effect_.Stop();
  impl_.animation_ = impl_.destroyed_animation_;
  impl_.render_box_ = impl_.destroyed_render_box_;
  impl_.sound_effect_ = impl_.destroyed_sound_effect_;
  impl_.sound_effect_.Play();
  impl_.animation_.Play();
  if(impl_.paused_)
  {
    impl_.animation_.Pause();
    impl_.sound_effect_.Pause();
  }
}

void HeroImpl::Render(void)
{
  display::BoundingBox destination(
    render_box_.x() + int(dynamics_.x()),
    render_box_.y() + int(dynamics_.y()),
    render_box_.w(),
    render_box_.h());
  animation_.Render(destination, 1.f, false, 0.);
}

HeroImpl::HeroImpl(json::JSON const& json, display::Window& window, Collision& collision, event::Signal& pause)
{
  json_t* moving_animation;
  json_t* moving_render_box;
  char const* moving_sound_effect;

  json_t* destroyed_animation;
  json_t* destroyed_render_box;
  char const* destroyed_sound_effect;

  json_t* collision_box;

  json.Unpack("{s{sososs}s{sososs}so}",
    "moving", "animation", &moving_animation, 
    "render box", &moving_render_box, 
    "sound effect", &moving_sound_effect,
    "destroyed", "animation", &destroyed_animation, 
    "render box", &destroyed_render_box, 
    "sound effect", &destroyed_sound_effect,
    "collision box", &collision_box);

  moving_animation_ = Animation(moving_animation, window);
  moving_render_box_ = display::BoundingBox(moving_render_box);
  moving_sound_effect_ = audio::Sound(moving_sound_effect);
    
  destroyed_animation_ = Animation(destroyed_animation, window);
  destroyed_render_box_ = display::BoundingBox(destroyed_render_box);
  destroyed_sound_effect_ = audio::Sound(destroyed_sound_effect);
  
  collision_box_ = display::BoundingBox(collision_box);
  paused_ = true;
  pause_ = event::Command(new PauseCommand(*this));
  pause.Add(pause_);

  animation_ = moving_animation_;
  animation_.Play(-1);
  animation_.Pause();
  render_box_ = moving_render_box_;
  sound_effect_ = moving_sound_effect_;
  sound_effect_.Play(1);
  sound_effect_.Pause();

  attack_ = event::Command(new AttackCommand(*this)); 
  event::button1.Add(attack_);
  up_ = event::Command(new UpCommand(*this)); 
  event::up.Add(up_);
  down_ = event::Command(new DownCommand(*this)); 
  event::down.Add(down_);
  left_ = event::Command(new LeftCommand(*this)); 
  event::left.Add(left_);
  right_ = event::Command(new RightCommand(*this)); 
  event::right.Add(right_);

  enemy_collision_ = event::Command(new EnemyCollisionCommand(*this));
  collision.Add(0, 1, collision_box_, enemy_collision_);
  dynamics_ = Dynamics(0.f, 0.f, 0.f, 0.f);
  dynamics_.Play();
  dynamics_.Pause();
}

float& Hero::x(void)
{
  return impl_->dynamics_.x();
}

float& Hero::y(void)
{
  return impl_->dynamics_.y();
}

void Hero::End(event::Command const& command)
{
  impl_->End(command);
}

Hero::Hero(std::string const& filename, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause) :
  Hero(json::JSON(filename), window, scene, collision, pause)
{
}

static Layer Render(std::shared_ptr<HeroImpl> impl)
{
  std::weak_ptr<HeroImpl> hero_ptr = impl;
  return [hero_ptr](void)
  {
    bool locked = false;
    if(auto hero = hero_ptr.lock())
    {
      hero->Render();
      locked = true;
    }
    return locked;
  };
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause) :
  impl_(new HeroImpl(json, window, collision, pause))
{
  scene.Add(Render(impl_), 0);
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