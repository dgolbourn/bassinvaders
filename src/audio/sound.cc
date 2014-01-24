#include "sound.h"
#include <unordered_map>
#include <set>
#include "SDL_mixer.h"
#include "chunk.h"
#include "mutex.h"

namespace audio
{
class SoundImpl : public std::enable_shared_from_this<SoundImpl>
{
public:
  SoundImpl(std::string const& filename);
  void Play(int repeats);
  void Pause(void) const;
  void Resume(void) const;
  void Stop(void) const;
  void Fade(int ms) const;
  void Volume(int volume);

  mix::Chunk chunk_;
  int volume_;
  std::set<int> channels_;
};

static int const default_volume = -1;
static sdl::Mutex mutex_;
typedef std::weak_ptr<SoundImpl> SoundPtr;
static std::unordered_map<int, SoundPtr> active_channels;

static void ChannelFinished(int channel)
{
  sdl::Lock lock(mutex_);
  auto sound_iter = active_channels.find(channel);
  if(sound_iter != active_channels.end())
  {
    if(auto sound = sound_iter->second.lock())
    {
      (void)Mix_Volume(channel, default_volume);
      (void)sound->channels_.erase(channel);
    }
    (void)active_channels.erase(sound_iter);
  }
}

static void InitChannelMixer(void)
{
  static bool initialised;
  if(!initialised)
  {
    Mix_ChannelFinished(ChannelFinished);
    initialised = true;
  }
}

SoundImpl::SoundImpl(std::string const& filename) : chunk_(filename), volume_(default_volume)
{
  InitChannelMixer();
}

void SoundImpl::Play(int repeats)
{
  sdl::Lock lock(mutex_);
  int channel = chunk_.Play(repeats, volume_);
  (void)channels_.insert(channel);
  active_channels[channel] = shared_from_this();
}

void SoundImpl::Pause(void) const
{
  for(auto channel : channels_)
  {
    Mix_Pause(channel);
  }
}

void SoundImpl::Resume(void) const
{
  for(auto channel : channels_)
  {
    Mix_Resume(channel);
  }
}

void SoundImpl::Stop(void) const
{
  for(auto channel : channels_)
  {
    (void)Mix_HaltChannel(channel);
  }
}

void SoundImpl::Fade(int ms) const
{
  for(auto channel : channels_)
  {
    (void)Mix_FadeOutChannel(channel, ms);
  }
}

void SoundImpl::Volume(int volume)
{
  volume_ = volume;
  for(auto channel : channels_)
  {
    (void)Mix_Volume(channel, volume_);
  }
}

Sound::Sound(std::string const& filename)
{
  impl_ = std::make_shared<SoundImpl>(filename);
}

void Sound::Play(int repeats)
{
  impl_->Play(repeats);
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
}
