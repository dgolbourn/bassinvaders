#include "hero.h"
#include "animation.h"
#include "bounding_box.h"
#include "sound.h"
#include "event.h"
#include "dynamics.h"

namespace game
{
class HeroImpl
{
public:
  HeroImpl(json::JSON const& json, display::Window& window);
  void End(event::Command const& command);
  Animation moving_animation_;
  display::BoundingBox moving_render_box_;
  audio::Sound moving_sound_effect_;
  Animation destroyed_animation_;
  display::BoundingBox destroyed_render_box_;
  audio::Sound destroyed_sound_effect_;
  display::BoundingBox collision_box_;
  bool paused_;
  Animation animation_;
  display::BoundingBox render_box_;
  audio::Sound sound_effect_;
  event::Command attack_;
  event::Command enemy_collision_;
  event::Signal end_;
  Dynamics dynamics_;
  void Render(void);
  void Pause(void);
  bool up_;
  bool down_;
  bool left_;
  bool right_;
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  void Attack(void);
  void EnemyCollision(void);
};

void HeroImpl::End(event::Command const& command)
{
  end_.Add(command);
}

void HeroImpl::Pause(void)
{
  if(paused_)
  {
    paused_ = false;
    animation_.Resume();
    sound_effect_.Resume();
    dynamics_.Resume();
  }
  else
  {
    paused_ = true;
    animation_.Pause();
    sound_effect_.Pause();
    dynamics_.Pause();
  }
}

void HeroImpl::Attack(void)
{
}

static float const dv = 0.1f;

void HeroImpl::Up(void)
{
  if(up_)
  {
    dynamics_.v() -= dv;
    up_ = false;
  }
  else
  {
    dynamics_.v() += dv;
    up_ = true;
  }
}

void HeroImpl::Down(void)
{
  if(down_)
  {
    dynamics_.v() += dv;
    down_ = false;
  }
  else
  {
    dynamics_.v() -= dv;
    down_ = true;
  }
}

void HeroImpl::Left(void)
{
  if(left_)
  {
    dynamics_.u() -= dv;
    left_ = false;
  }
  else
  {
    dynamics_.u() += dv;
    left_ = true;
  }
}

void HeroImpl::Right(void)
{
  if(right_)
  {
    dynamics_.u() += dv;
    right_ = false;
  }
  else
  {
    dynamics_.u() -= dv;
    right_ = true;
  }
}

void HeroImpl::EnemyCollision(void)
{
  sound_effect_.Stop();
  animation_ = destroyed_animation_;
  render_box_ = destroyed_render_box_;
  sound_effect_ = destroyed_sound_effect_;
  sound_effect_.Play();
  animation_.Play();
  if(paused_)
  {
    animation_.Pause();
    sound_effect_.Pause();
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

HeroImpl::HeroImpl(json::JSON const& json, display::Window& window)
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

  animation_ = moving_animation_;
  animation_.Play(-1);
  animation_.Pause();
  render_box_ = moving_render_box_;
  sound_effect_ = moving_sound_effect_;
  sound_effect_.Play(1);
  sound_effect_.Pause();
  up_ = true;
  down_ = true;
  left_ = true;
  right_ = true;
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

typedef void (HeroImpl::*HeroImplMethod)(void);
static Layer Bind(std::shared_ptr<HeroImpl>& impl, HeroImplMethod method)
{
  std::weak_ptr<HeroImpl> hero_ptr = impl;
  return [=](void)
  {
    bool locked = false;
    if(auto hero = hero_ptr.lock())
    {
      HeroImpl* ptr = hero.get();
      (ptr->*method)();
      locked = true;
    }
    return locked;
  };
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause) :
  impl_(new HeroImpl(json, window))
{
  scene.Add(Bind(impl_, &HeroImpl::Render), 0);
  pause.Add(Bind(impl_, &HeroImpl::Pause));
  event::up.Add(Bind(impl_, &HeroImpl::Up));
  event::down.Add(Bind(impl_, &HeroImpl::Down));
  event::left.Add(Bind(impl_, &HeroImpl::Left));
  event::right.Add(Bind(impl_, &HeroImpl::Right));
  event::button1.Add(Bind(impl_, &HeroImpl::Attack));
  collision.Add(0, 1, impl_->collision_box_, Bind(impl_, &HeroImpl::EnemyCollision));
}

Hero::Hero(std::string const& filename, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause) :
Hero(json::JSON(filename), window, scene, collision, pause)
{
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