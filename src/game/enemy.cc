#include "enemy.h"
#include "animation.h"
#include "bounding_box.h"
#include "sound.h"
#include "event.h"
#include "dynamics.h"

namespace game
{
class EnemyImpl
{
public:
  EnemyImpl(json::JSON const& json, display::Window& window);
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
  void HeroCollision(void);
};

void EnemyImpl::End(event::Command const& command)
{
  end_.Add(command);
}

void EnemyImpl::Pause(void)
{
  if(paused_)
  {
    paused_ = false;
    animation_.Resume();
    sound_effect_.Resume();
  }
  else
  {
    paused_ = true;
    animation_.Pause();
    sound_effect_.Pause();
  }
}

void EnemyImpl::Attack(void)
{
}

static float const dv = 0.1f;

void EnemyImpl::Up(void)
{
  if(up_)
  {
    dynamics_.v(-dv);
    up_ = false;
  }
  else
  {
    dynamics_.v(dv);
    up_ = true;
  }
}

void EnemyImpl::Down(void)
{
  if(down_)
  {
    dynamics_.v(dv);
    down_ = false;
  }
  else
  {
    dynamics_.v(-dv);
    down_ = true;
  }
}

void EnemyImpl::Left(void)
{
  if(left_)
  {
    dynamics_.u(-dv);
    left_ = false;
  }
  else
  {
    dynamics_.u(dv);
    left_ = true;
  }
}

void EnemyImpl::Right(void)
{
  if(right_)
  {
    dynamics_.u(dv);
    right_ = false;
  }
  else
  {
    dynamics_.u(-dv);
    right_ = true;
  }
}

void EnemyImpl::HeroCollision(void)
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

void EnemyImpl::Render(void)
{
  display::BoundingBox destination(
    render_box_.x() + int(dynamics_.x()),
    render_box_.y() + int(dynamics_.y()),
    render_box_.w(),
    render_box_.h());
  animation_.Render(destination, 1.f, false, 0.);
}

EnemyImpl::EnemyImpl(json::JSON const& json, display::Window& window)
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

  /*moving_animation_ = Animation(moving_animation, window);
  moving_render_box_ = display::BoundingBox(moving_render_box);
  moving_sound_effect_ = audio::Sound(moving_sound_effect);
    
  destroyed_animation_ = Animation(destroyed_animation, window);
  destroyed_render_box_ = display::BoundingBox(destroyed_render_box);
  destroyed_sound_effect_ = audio::Sound(destroyed_sound_effect);
  */
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
}

void Enemy::Position(int x, int y)
{
}

game::Position Enemy::Position(void)
{
  return game::Position(int(impl_->dynamics_.x()), int(impl_->dynamics_.y()));
}

void Enemy::End(event::Command const& command)
{
  impl_->End(command);
}

typedef void (EnemyImpl::*EnemyImplMethod)(void);
static event::Command Bind(std::shared_ptr<EnemyImpl> const& impl, EnemyImplMethod method)
{
  std::weak_ptr<EnemyImpl> Enemy_ptr = impl;
  return [=](void)
  {
    bool locked = false;
    if(auto Enemy = Enemy_ptr.lock())
    {
      EnemyImpl* ptr = Enemy.get();
      (ptr->*method)();
      locked = true;
    }
    return locked;
  };
}

Enemy::Enemy(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause, Role& role) :
  impl_(new EnemyImpl(json, window))
{
  scene.Add(Bind(impl_, &EnemyImpl::Render), 0);
  pause.Add(Bind(impl_, &EnemyImpl::Pause));
  role.Up(Bind(impl_, &EnemyImpl::Up));
  role.Down(Bind(impl_, &EnemyImpl::Down));
  role.Left(Bind(impl_, &EnemyImpl::Left));
  role.Right(Bind(impl_, &EnemyImpl::Right));
  role.Attack(Bind(impl_, &EnemyImpl::Attack));
//  collision.Add(1, 0, impl_->collision_box_, Bind(impl_, &EnemyImpl::HeroCollision));
}

Enemy::Enemy(std::string const& filename, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause, Role& role) :
Enemy(json::JSON(filename), window, scene, collision, pause, role)
{
}
}