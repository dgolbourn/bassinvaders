#include "enemy.h"
#include "animation.h"
#include "bounding_box.h"
#include "sound.h"
#include "event.h"

namespace game
{
class EnemyImpl
{
public:
  EnemyImpl(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause);
  void End(event::Command const& command);
  Animation moving_animation_;
  display::BoundingBox moving_render_box_;
  audio::Sound moving_sound_effect_;
  Animation destroyed_animation_;
  display::BoundingBox destroyed_render_box_;
  audio::Sound destroyed_sound_effect_;
  display::BoundingBox collision_box_;
  event::Command render_;
  event::Command pause_;
  bool paused_;
  Animation animation_;
  display::BoundingBox render_box_;
  audio::Sound sound_effect_;
  int x_;
  int y_;
  event::Command hero_collision_;
  class RenderCommand;
  class PauseCommand;
  event::Signal end_;
};

class EnemyImpl::RenderCommand : public event::CommandImpl
{
public:
  RenderCommand(EnemyImpl& impl);
  void operator()(void) final;
  EnemyImpl& impl_;
};

EnemyImpl::RenderCommand::RenderCommand(EnemyImpl& impl) : impl_(impl)
{
}

void EnemyImpl::RenderCommand::operator()(void)
{
  display::BoundingBox destination(
    impl_.render_box_.x() + impl_.x_, 
    impl_.render_box_.y() + impl_.y_,
    impl_.render_box_.w(),
    impl_.render_box_.h());
  impl_.animation_.Render(destination);
}

class EnemyImpl::PauseCommand : public event::CommandImpl
{
public:
  PauseCommand(EnemyImpl& impl);
  void operator()(void) final;
  EnemyImpl& impl_;
};

EnemyImpl::PauseCommand::PauseCommand(EnemyImpl& impl) : impl_(impl)
{
}

void EnemyImpl::PauseCommand::operator()(void)
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

class HeroCollisionCommand : public event::CommandImpl
{
public:
  HeroCollisionCommand(EnemyImpl& impl);
  void operator()(void) final;
  EnemyImpl& impl_;
};

HeroCollisionCommand::HeroCollisionCommand(EnemyImpl& impl) : impl_(impl)
{
}

void HeroCollisionCommand::operator()(void)
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

EnemyImpl::EnemyImpl(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause)
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
  render_ = event::Command(new RenderCommand(*this));
  scene.Add(render_, 0);
  paused_ = true;
  pause_ = event::Command(new PauseCommand(*this));
  pause.Add(pause_);

  animation_ = moving_animation_;
  animation_.Play(-1);
  animation_.Pause();
  render_box_ = moving_render_box_;
  sound_effect_ = moving_sound_effect_;
  sound_effect_.Play(-1);
  sound_effect_.Pause();

  hero_collision_ = event::Command(new HeroCollisionCommand(*this));
  collision.Add(1, 0, collision_box_, hero_collision_);
}

void EnemyImpl::End(event::Command const& command)
{
  end_.Add(command);
}

int& Enemy::x(void)
{
  return impl_->x_;
}

int& Enemy::y(void)
{
  return impl_->y_;
}

void Enemy::End(event::Command const& command)
{
  impl_->End(command);
}

Enemy::Enemy(std::string const& filename, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause) :
  impl_(new EnemyImpl(json::JSON(filename), window, scene, collision, pause))
{
}

Enemy::Enemy(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause) :
  impl_(new EnemyImpl(json, window, scene, collision, pause))
{
}

Enemy::Enemy(Enemy const& other) : impl_(other.impl_)
{
}

Enemy::Enemy(Enemy&& other) : impl_(std::move(other.impl_))
{
}

Enemy::~Enemy(void)
{
}

Enemy& Enemy::operator=(Enemy other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}