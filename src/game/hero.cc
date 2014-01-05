#include "hero.h"
#include "bounding_box.h"
#include "event.h"
#include "dynamics.h"
#include "bind.h"
#include "state.h"

namespace game
{
class HeroImpl
{
public:
  HeroImpl(json::JSON const& json, display::Window& window);
  void End(event::Command const& command);
  State moving_;
  State destroyed_;
  State spawn_;
  State idle_;
  State hit_;
  State current_;
  display::BoundingBox render_box_;
  display::BoundingBox collision_box_;
  bool paused_;
  event::Signal end_;
  event::Signal life_signal_;
  game::Position position_;
  Dynamics dynamics_;
  int x_sign_;
  int x_facing_;
  int y_sign_;
  int y_facing_;
  int life_;
  std::mutex mutex_;
  void Render(void);
  void Pause(void);
  void Resume(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  void Update(void);
  void Attack(void);
  RulesCollision::Rules EnemySend(void);
  void EnemyReceive(RulesCollision::Rules const& rules);
  void SignalEnd(void);
  void Reset(void);
  void Position(Dynamics::Position const& position);
  void Change(State& next);
  void Life(Hero::Command command);
};

void HeroImpl::End(event::Command const& command)
{
  end_.Add(command);
}

void HeroImpl::Pause(void)
{
  paused_ = true;
  current_.Pause();
  dynamics_.Pause();
}

void HeroImpl::Resume(void)
{
  paused_ = false;
  current_.Resume();
  dynamics_.Resume();
}

void HeroImpl::Attack(void)
{
}

static float const dv = 0.25f;
static float const sqrt1_2 = std::sqrt(0.5f);

void HeroImpl::Up(void)
{
  --y_sign_;
  Update();
}

void HeroImpl::Down(void)
{
  ++y_sign_;
  Update();
}

void HeroImpl::Left(void)
{
  --x_sign_;
  Update();
}

void HeroImpl::Right(void)
{
  ++x_sign_;
  Update();
}

void HeroImpl::Update(void)
{
  float v = dv;
  if(x_sign_ && y_sign_)
  {
    v *= sqrt1_2;
  }
  if(x_sign_)
  {
    x_facing_ = x_sign_;
  }

  dynamics_.u(float(x_sign_) * v);
  dynamics_.v(float(y_sign_) * v);

  if(x_sign_ || y_sign_)
  {
    if(current_ == idle_)
    {
      Change(moving_);
    }
  }
  else
  {
    if(current_ == moving_)
    {
      Change(idle_);
    }
  }
}

RulesCollision::Rules HeroImpl::EnemySend(void)
{
  return RulesCollision::Rules();
}

void HeroImpl::EnemyReceive(RulesCollision::Rules const& rules)
{
  life_ -= rules.first;
  if(life_ <= 0)
  {
    life_ = 0;
    Change(destroyed_);
  }
  else
  {
    Change(hit_);
  }
  life_signal_();
}

void HeroImpl::Change(State& next)
{
  current_.Stop();
  current_ = next;
  current_.Play();
  if(paused_)
  {
    current_.Pause();
  }
}

static void BoxUpdate(display::BoundingBox const& source, display::BoundingBox& destination, Position const& position, int x_facing)
{
  display::BoundingBox temp = source.Copy();
  int sourcex = temp.x();
  temp.x(temp.x() + position.first);
  temp.y(temp.y() + position.second);
  if(x_facing < 0)
  {
    temp.x(temp.x() - 2 * sourcex);
    temp.w(-temp.w());
  }
  destination.Copy(temp);
}

void HeroImpl::Position(Dynamics::Position const& position)
{
  position_.first = int(position.first); 
  position_.second = int(position.second);
  BoxUpdate(current_.Render(), render_box_, position_, x_facing_);
  BoxUpdate(current_.Collision(), collision_box_, position_, x_facing_);
}

void HeroImpl::Render(void)
{
  current_.Render(render_box_);
}

void HeroImpl::Reset(void)
{
  Change(idle_); 
  Update();
}

void HeroImpl::SignalEnd(void)
{
  end_();
}

HeroImpl::HeroImpl(json::JSON const& json, display::Window& window)
{
  json_t* moving;
  json_t* destroyed;
  json_t* spawn;
  json_t* idle;
  json_t* hit;

  json.Unpack("{sososososo}",
    "moving", &moving,
    "destroyed", &destroyed,
    "spawn", &spawn,
    "idle", &idle,
    "hit", &hit);
  
  paused_ = true;
  moving_ = State(moving, window);
  destroyed_ = State(destroyed, window);
  idle_ = State(idle, window);
  hit_ = State(hit, window);
  spawn_ = State(spawn, window);
  current_ = spawn_;
  current_.Play();
  current_.Pause();
  collision_box_ = current_.Collision().Copy();
  render_box_ = current_.Render().Copy();
  position_ = game::Position(0, 0);
  dynamics_ = Dynamics(0.f, 0.f, 0.f, 0.f);
  dynamics_.Play();
  dynamics_.Pause();

  x_sign_ = 0;
  y_sign_ = 0;
  x_facing_ = 0;
  y_facing_ = 0;
  life_ = 100;
}

void HeroImpl::Life(Hero::Command command)
{
  life_signal_.Add([=](){return command(life_);});
}

void Hero::Position(game::Position const& position)
{
  thread::Lock lock(impl_->mutex_);
  impl_->position_ = position;
  impl_->dynamics_.x(float(position.first));
  impl_->dynamics_.y(float(position.second));
}

game::Position Hero::Position(void)
{
  thread::Lock lock(impl_->mutex_);
  return impl_->position_;
}

void Hero::End(event::Command const& command)
{
  thread::Lock lock(impl_->mutex_);
  impl_->End(command);
}

void Hero::Life(Command const& command)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Life(command);
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, RulesCollision& collision)
{
  impl_ = std::make_shared<HeroImpl>(json, window);
  thread::Lock lock(impl_->mutex_);
  impl_->hit_.End(event::Bind(&HeroImpl::Reset, impl_));
  impl_->destroyed_.End(event::Bind(&HeroImpl::SignalEnd, impl_));
  impl_->dynamics_.Add(event::Bind(&HeroImpl::Position, impl_));
  impl_->spawn_.End(event::Bind(&HeroImpl::Reset, impl_));
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
  RulesCollision::Channel::Send send(event::Bind(&HeroImpl::EnemySend, impl_));
  RulesCollision::Channel::Receive receive(event::Bind(&HeroImpl::EnemyReceive, impl_));
  RulesCollision::Channel::Pair channel(send, receive);
  collision.Add(0, impl_->collision_box_, channel);
}
}