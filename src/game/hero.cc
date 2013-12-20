#include "hero.h"
#include "animation.h"
#include "bounding_box.h"
#include "sound.h"
#include "event.h"
#include "dynamics.h"
#include "bind.h"

namespace game
{
class State
{
public:
  Animation animation_;
  display::BoundingBox render_box_;
  audio::Sound sound_effect_;
  display::BoundingBox collision_box_;
};

class HeroImpl
{
public:
  HeroImpl(json::JSON const& json, display::Window& window);
  void End(event::Command const& command);
  State Moving;
  State Destroyed;
  State Current;
  display::BoundingBox render_box_;
  display::BoundingBox collision_box_;
  bool paused_;
  event::Signal end_;
  game::Position position_;
  Dynamics dynamics_;
  int x_sign_;
  int y_sign_;
  void Render(void);
  void Pause(void);
  void Resume(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  void Velocity(void);
  void Attack(void);
  void EnemyCollision(void);
  bool Position(Dynamics::Position const& position);
};

void HeroImpl::End(event::Command const& command)
{
  end_.Add(command);
}

void HeroImpl::Pause(void)
{
  paused_ = true;
  Current.animation_.Pause();
  Current.sound_effect_.Pause();
  dynamics_.Pause();
}

void HeroImpl::Resume(void)
{
  paused_ = false;
  Current.animation_.Resume();
  Current.sound_effect_.Resume();
  dynamics_.Resume();
}

void HeroImpl::Attack(void)
{
}

static float const dv = 0.1f;
static float const sqrt1_2 = std::sqrt(0.5f);

void HeroImpl::Up(void)
{
  y_sign_ -= 1;
  Velocity();
}

void HeroImpl::Down(void)
{
  y_sign_ += 1;
  Velocity();
}

void HeroImpl::Left(void)
{
  x_sign_ -= 1;
  Velocity();
}

void HeroImpl::Right(void)
{
  x_sign_ += 1;
  Velocity();
}

void HeroImpl::Velocity(void)
{
  float v = dv;
  if(x_sign_ && y_sign_)
  {
    v *= sqrt1_2;
  }

  dynamics_.u(float(x_sign_) * v);
  dynamics_.v(float(y_sign_) * v);
}

void HeroImpl::EnemyCollision(void)
{
  Current.sound_effect_.Stop();
  Current.animation_.Pause();
  Current = Destroyed;
  Current.sound_effect_.Play();
  Current.animation_.Play();
  Current.animation_.End([this](){this->end_.Notify(); return false;});
  if(paused_)
  {
    Current.animation_.Pause();
    Current.sound_effect_.Pause();
  }
}

bool HeroImpl::Position(Dynamics::Position const& position)
{
  position_.first = int(position.first); 
  position_.second = int(position.second);

  render_box_.Copy(Current.render_box_);
  render_box_.x(render_box_.x() + position_.first);
  render_box_.y(render_box_.y() + position_.second);

  if(Current.collision_box_)
  {
    collision_box_.Copy(Current.collision_box_);
    collision_box_.x(collision_box_.x() + position_.first);
    collision_box_.y(collision_box_.y() + position_.second);
  }
  else
  {
    collision_box_ = display::BoundingBox();
  }
  return true;
}

void HeroImpl::Render(void)
{
  Current.animation_.Render(render_box_, 1.f, false, 0.);
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

  Moving.animation_ = Animation(moving_animation, window);
  Moving.render_box_ = display::BoundingBox(moving_render_box);
  Moving.sound_effect_ = audio::Sound(moving_sound_effect);
  Moving.collision_box_ = display::BoundingBox(collision_box);

  Destroyed.animation_ = Animation(destroyed_animation, window);
  Destroyed.render_box_ = display::BoundingBox(destroyed_render_box);
  Destroyed.sound_effect_ = audio::Sound(destroyed_sound_effect);
  Destroyed.collision_box_ = display::BoundingBox();
  
  paused_ = true;

  Current = Moving;
  Current.animation_.Play(-1);
  Current.animation_.Pause();
  Current.sound_effect_.Play(0);
  Current.sound_effect_.Pause();
  collision_box_ = display::BoundingBox(collision_box);
  render_box_ = display::BoundingBox(moving_render_box);
  position_ = game::Position(0, 0);
  dynamics_ = Dynamics(0.f, 0.f, 0.f, 0.f);
  dynamics_.Add(std::bind(&HeroImpl::Position, this, std::placeholders::_1));
  dynamics_.Play();
  dynamics_.Pause();

  x_sign_ = 0;
  y_sign_ = 0;
}

void Hero::Position(game::Position const& position)
{
  impl_->position_ = position;
  impl_->dynamics_.x(float(position.first));
  impl_->dynamics_.y(float(position.second));
}

game::Position Hero::Position(void)
{
  return impl_->position_;
}

void Hero::End(event::Command const& command)
{
  impl_->End(command);
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision)
{
  impl_ = std::make_shared<HeroImpl>(json, window);

  scene.Add(event::Bind(&HeroImpl::Render, impl_), 0);
  event::pause.first.Add(event::Bind(&HeroImpl::Pause, impl_));
  event::pause.second.Add(event::Bind(&HeroImpl::Resume, impl_));
  event::up.first.Add(event::Bind(&HeroImpl::Up, impl_));
  event::up.second.Add(event::Bind(&HeroImpl::Down, impl_));
  event::down.first.Add(event::Bind(&HeroImpl::Down, impl_));
  event::down.second.Add(event::Bind(&HeroImpl::Up, impl_));
  event::left.first.Add(event::Bind(&HeroImpl::Left, impl_));
  event::left.second.Add(event::Bind(&HeroImpl::Right, impl_));
  event::right.first.Add(event::Bind(&HeroImpl::Right, impl_));
  event::right.second.Add(event::Bind(&HeroImpl::Left, impl_));
  event::button1.first.Add(event::Bind(&HeroImpl::Attack, impl_));
  collision.Add(0, 1, impl_->collision_box_, event::Bind(&HeroImpl::EnemyCollision, impl_));
}

Hero::Hero(std::string const& filename, display::Window& window, Scene& scene, Collision& collision) :
Hero(json::JSON(filename), window, scene, collision)
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