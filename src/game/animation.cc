#include "animation.h"
#include "timer.h"
#include "jansson.h"
#include "json_exception.h"
#include "bounding_box.h"
#include "bind.h"
#include <vector>
#include "thread.h"

namespace game
{
class AnimationImpl
{
public:
  AnimationImpl(json::JSON const& json, display::Window& window);
  bool Next(void);
  void Render(display::BoundingBox const& destination, float parallax, bool tile, double angle) const;
  void Pause(void);
  void Resume(void);
  void Play(int loops, bool end_on_first);
  void End(event::Command const& command);

  display::Texture texture_;
  event::Timer timer_;
  std::vector<display::BoundingBox> frames_;
  std::vector<display::BoundingBox>::iterator frame_;
  std::mutex mutex_;
};

AnimationImpl::AnimationImpl(json::JSON const& json, display::Window& window)
{
  char const* sprite_sheet;
  int interval;
  int width;
  int height;
  json_t* frames;

  json.Unpack("{sssisisiso}", 
    "sprite sheet", &sprite_sheet,
    "interval", &interval,
    "width", &width,
    "height", &height,
    "frames", &frames);

  texture_ = window.Load(std::string(sprite_sheet));
  timer_ = event::Timer(interval);
  frames_ = std::vector<display::BoundingBox>(json_array_size(frames));
  frame_ = frames_.begin();

  size_t index;
  json_t* value;
  json_array_foreach(frames, index, value)
  {
    int x;
    int y;
    json::JSON(value).Unpack("[ii]", &x, &y);
    *frame_ = display::BoundingBox(x, y, width, height);
    ++frame_;
  }
  frame_ = frames_.begin(); 
}

bool AnimationImpl::Next(void)
{
  ++frame_;
  if(frame_ == frames_.end())
  {
    frame_ = frames_.begin();
  }
  return true;
}

void AnimationImpl::Render(display::BoundingBox const& destination, float parallax, bool tile, double angle) const
{
  texture_(*frame_, destination, parallax, tile, angle);
}

void AnimationImpl::Pause(void)
{
  timer_.Pause();
}

void AnimationImpl::Resume(void)
{
  timer_.Resume();
}

void AnimationImpl::Play(int loops, bool end_on_first)
{
  int const frames = (loops + 1) * frames_.size() - 2 + int(end_on_first);
  timer_.Play(frames);
  frame_ = frames_.begin(); 
}

void AnimationImpl::End(event::Command const& command)
{
  timer_.End(command);
}

Animation::Animation(std::string const& filename, display::Window& window) : Animation(json::JSON(filename), window)
{
}

Animation::Animation(json::JSON const& json, display::Window& window)
{
  impl_ = std::make_shared<AnimationImpl>(json, window);
  thread::Lock lock(impl_->mutex_);
  impl_->timer_.Add(event::Bind(&AnimationImpl::Next, impl_));
}

void Animation::Render(display::BoundingBox const& destination, float parallax, bool tile, double angle) const
{
  thread::Lock lock(impl_->mutex_);
  impl_->Render(destination, parallax, tile, angle);
}

void Animation::Pause(void)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Pause();
}

void Animation::Resume(void)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Resume();
}

void Animation::Play(int loops, bool end_on_first)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Play(loops, end_on_first);
}

void Animation::End(event::Command const& command)
{
  thread::Lock lock(impl_->mutex_);
  impl_->End(command);
}

Animation::Animation(void)
{
}

Animation::Animation(Animation const& other) : impl_(other.impl_)
{
}

Animation::Animation(Animation&& other) : impl_(std::move(other.impl_))
{
}

Animation::~Animation(void)
{
}

Animation& Animation::operator=(Animation other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}