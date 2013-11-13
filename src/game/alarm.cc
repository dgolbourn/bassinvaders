#include "alarm.h"
#include "timer.h"

namespace game
{
class AlarmImpl
{
public:
  AlarmImpl(json::JSON const& json, audio::Mixer& mixer);
  void Pause(void);
  void Resume(void);

  audio::Sound sound_;
  event::Command command_;
  event::Timer timer_;
};

class AlarmCommand : public event::CommandImpl
{
public:
  AlarmCommand(AlarmImpl& impl);
  void operator()(void) final;
  AlarmImpl& impl_;
};

AlarmCommand::AlarmCommand(AlarmImpl& impl) : impl_(impl)
{
}

void AlarmCommand::operator()(void)
{
  impl_.sound_.Play();
}

AlarmImpl::AlarmImpl(json::JSON const& json, audio::Mixer& mixer)
{
  char const* sound;
  int interval;

  json.Unpack("{sssi}", 0, "sound", &sound, "interval", &interval);

  sound_ = mixer.Load(sound);
  command_ = event::Command(new AlarmCommand(*this));
  timer_ = event::Timer(interval);
  timer_.Pause();
  timer_.Add(command_);
}

void AlarmImpl::Pause(void)
{
  timer_.Pause();
}

void AlarmImpl::Resume(void)
{
  timer_.Resume();
}

Alarm::Alarm(std::string const& filename, audio::Mixer& mixer)
{
  impl_ = std::shared_ptr<AlarmImpl>(new AlarmImpl(json::JSON(filename), mixer));
}

Alarm::Alarm(json::JSON const& json, audio::Mixer& mixer)
{
  impl_ = std::shared_ptr<AlarmImpl>(new AlarmImpl(json, mixer));
}

void Alarm::Pause(void)
{
  impl_->Pause();
}

void Alarm::Resume(void)
{
  impl_->Resume();
}

Alarm::Alarm(void)
{
}

Alarm::Alarm(Alarm const& other) : impl_(other.impl_)
{
}

Alarm::Alarm(Alarm&& other) : impl_(std::move(other.impl_))
{
}

Alarm::~Alarm(void)
{
}

Alarm& Alarm::operator=(Alarm other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}