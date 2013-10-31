#include "animation.h"
#include "texture.h"
#include "signal.h"
#include "timer.h"
#include "jansson.h"
#include "json_exception.h"
#include "json.h"
#include "bounding_box.h"

#include <string>
#include <list>
#include <mutex>

namespace game
{

class AnimationImpl
{
public:
  AnimationImpl(std::string& filename, display::Window& window);
  ~AnimationImpl(void);

  void Load(std::string& filename, display::Window& window);
  void Next(void);
  void Render(display::BoundingBox& destination);
  void Pause(void);
  void Resume(void);
  void Restart(void);

  display::Texture texture_;
  event::Observer trigger_;
  event::Timer timer_;
  std::list<display::BoundingBox> frames_;
  std::list<display::BoundingBox>::iterator frame_;
  std::mutex mutex_;
};

class AnimationCallback : public event::Notification
{
public:
  AnimationCallback(AnimationImpl& impl);
  void operator()(void);
  AnimationImpl& impl_;
};

AnimationCallback::AnimationCallback(AnimationImpl& impl) : impl_(impl)
{
}

void AnimationCallback::operator()(void)
{
  impl_.Next();
}

AnimationImpl::AnimationImpl(std::string& filename, display::Window& window)
{
  Load(filename, window);
}

AnimationImpl::~AnimationImpl(void)
{
}

void AnimationImpl::Load(std::string& filename, display::Window& window)
{
  json::JSON json = json::Load(filename);

  char const* sprite_sheet;
  int interval;
  int height;
  int width;
  json_t* frames;

  json_error_t error;
  if(json_unpack_ex(json.Get(), &error, JSON_STRICT, "{sssisisiso}", 
    "sprite sheet", &sprite_sheet, 
    "interval", &interval,
    "width", &width,
    "height", &height,
    "frames", &frames) == -1)
  { 
    throw json::Exception(error);
  }
  else
  {
    texture_ = window.Load(std::string(sprite_sheet));
    timer_ = event::Timer(interval);
    timer_.Pause();
    trigger_ = event::Observer(new AnimationCallback(*this));
    timer_.Signal().Subscribe(trigger_);

    size_t index;
    json_t* value;
    json_array_foreach(frames, index, value)
    {
      int x;
      int y;
      if(json_unpack_ex(value, &error, JSON_STRICT, "[ii]", &x, &y) == -1)
      {
        throw json::Exception(error);
      }
      else
      {
        frames_.push_back(display::BoundingBox(x, y, width, height));
      }
    }
    frame_ = frames_.begin(); 
  }
}

void AnimationImpl::Next(void)
{
  mutex_.lock();
  frame_++;
  if(frame_ == frames_.end())
  {
    frame_ = frames_.begin();
  }
  mutex_.unlock();
}

void AnimationImpl::Render(display::BoundingBox& destination)
{
  mutex_.lock();
  texture_.Render(*frame_, destination);
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

void AnimationImpl::Restart(void)
{
  mutex_.lock();
  timer_.Resume();
  frame_ = frames_.begin(); 
  mutex_.unlock();
}

Animation::Animation(std::string& filename, display::Window& window)
{
  impl_ = std::shared_ptr<AnimationImpl>(new AnimationImpl(filename, window));
}

void Animation::Render(display::BoundingBox& destination)
{
  impl_->Render(destination);
}

void Animation::Pause(void)
{
  impl_->Pause();
}

void Animation::Resume(void)
{
  impl_->Resume();
}

void Animation::Restart(void)
{
  impl_->Restart();
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