#include "sound.h"
#include <unordered_map>
#include "SDL_mixer.h"
#include "chunk.h"
#include "signal.h"

namespace audio
{
typedef std::weak_ptr<class SoundImpl> SoundPtr;

class SoundImpl
{
public:
  SoundImpl(std::string const& filename);
  void Play(int repeats, SoundPtr const& impl);
  void Pause(void) const;
  void Resume(void) const;
  void Stop(void) const;
  void Fade(int ms) const;
  void Volume(int volume);
  void End(event::Command const& command);

  mix::Chunk chunk_;
  int volume_;
  int channel_;
  event::Signal signal_;
};

static int const no_channel = -1;
static int const default_volume = -1;

static std::unordered_map<int, SoundPtr> active_channels;

static void ChannelFinished(int channel)
{
  auto sound_iter = active_channels.find(channel);
  if(sound_iter != active_channels.end())
  {
    if(auto sound = sound_iter->second.lock())
    {
      sound->signal_();
      Mix_Volume(sound->channel_, default_volume);
      sound->channel_ = no_channel;
      (void)active_channels.erase(sound_iter);
    }
  }
}

SoundImpl::SoundImpl(std::string const& filename) : chunk_(filename), volume_(default_volume), channel_(no_channel)
{
  static bool initialised;
  if(!initialised)
  {
    Mix_ChannelFinished(ChannelFinished);
    initialised = true;
  }
}

void SoundImpl::Play(int repeats, SoundPtr const& impl)
{
  if(channel_ == no_channel)
  {
    channel_ = chunk_.Play(repeats, volume_);
    active_channels[channel_] = impl;
  }
}

void SoundImpl::Pause(void) const
{
  if(channel_ != no_channel)
  {
    Mix_Pause(channel_);
  }
}

void SoundImpl::Resume(void) const
{
  if(channel_ != no_channel)
  {
    Mix_Resume(channel_);
  }
}

void SoundImpl::Stop(void) const
{
  if(channel_ != no_channel)
  {
    (void)Mix_HaltChannel(channel_);
  }
}

void SoundImpl::Fade(int ms) const
{
  if(channel_ != no_channel)
  {
    (void)Mix_FadeOutChannel(channel_, ms);
  }
}

void SoundImpl::Volume(int volume)
{
  volume_ = volume;
  (void)Mix_Volume(channel_, volume_);
}

void SoundImpl::End(event::Command const& command)
{
  signal_.Add(command);
}

Sound::Sound(std::string const& filename)
{
  impl_ = std::make_shared<SoundImpl>(filename);
}

void Sound::Play(int repeats)
{
  impl_->Play(repeats, impl_);
}

void Sound::Pause(void) const
{
  impl_->Pause();
}

void Sound::Resume(void) const
{
  impl_->Resume();
}

void Sound::Stop(void) const
{
  impl_->Stop();
}

void Sound::Fade(int ms) const
{
  impl_->Fade(ms);
}

void Sound::Volume(int volume)
{
  impl_->Volume(volume);
}

void Sound::End(event::Command const& command)
{
  impl_->End(command);
}
}
