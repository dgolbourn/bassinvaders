#include "animation.h"
#include "timer.h"
#include "jansson.h"
#include "json_exception.h"
#include "bounding_box.h"

#include <vector>
#include <mutex>

namespace game
{
class AnimationImpl
{
public:
  AnimationImpl(json::JSON const& json, display::Window& window);
  void Next(void);
  void Render(display::BoundingBox const& destination, float parallax, bool tile, double angle);
  void Pause(void);
  void Resume(void);
  void Play(int loops);
  void End(event::Command const& command);

  display::Texture texture_;
  event::Command command_;
  event::Timer timer_;
  std::vector<display::BoundingBox> frames_;
  std::vector<display::BoundingBox>::iterator frame_;
  std::mutex mutex_;
};

class AnimationCommand : public event::CommandImpl
{
public:
  AnimationCommand(AnimationImpl& impl);
  void operator()(void) final;
  AnimationImpl& impl_;
};

AnimationCommand::AnimationCommand(AnimationImpl& impl) : impl_(impl)
{
}

void AnimationCommand::operator()(void)
{
  impl_.Next();
}

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
  command_ = event::Command(new AnimationCommand(*this));
  timer_ = event::Timer(interval);
  timer_.Add(command_);
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

void AnimationImpl::Next(void)
{
  mutex_.lock();
  ++frame_;
  if(frame_ == frames_.end())
  {
    frame_ = frames_.begin();
  }
  mutex_.unlock();
}

void AnimationImpl::Render(display::BoundingBox const& destination, float parallax, bool tile, double angle)
{
  mutex_.lock();
  texture_(*frame_, destination, parallax, tile, angle);
  mutex_.unlock();
}

void AnimationImpl::Pause(void)
{
  timer_.Pause();
}

void AnimationImpl::Resume(void)
{
  timer_.Resume();
}

void AnimationImpl::Play(int loops)
{
  mutex_.lock();
  int const frames = (loops + 1) * frames_.size() - 1;
  timer_.Play(frames);
  frame_ = frames_.begin(); 
  mutex_.unlock();
}

void AnimationImpl::End(event::Command const& command)
{
  timer_.End(command);
}

Animation::Animation(std::string const& filename, display::Window& window) : impl_(new AnimationImpl(json::JSON(filename), window))
{
}

Animation::Animation(json::JSON const& json, display::Window& window) : impl_(new AnimationImpl(json, window))
{
}

void Animation::Render(display::BoundingBox const& destination, float parallax, bool tile, double angle)
{
  impl_->Render(destination, parallax, tile, angle);
}

void Animation::Pause(void)
{
  impl_->Pause();
}

void Animation::Resume(void)
{
  impl_->Resume();
}

void Animation::Play(int loops)
{
  impl_->Play(loops);
}

void Animation::End(event::Command const& command)
{
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