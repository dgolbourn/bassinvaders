#include "sound.h"
#include <unordered_map>
#include "SDL_mixer.h"
#include "chunk.h"
#include "thread.h"

namespace audio
{
class SoundImpl : public std::enable_shared_from_this<SoundImpl>
{
public:
  SoundImpl(std::string const& filename);
  void Play(int repeats);
  void Pause(void);
  void Resume(void);
  void Stop(void);
  void Fade(int ms);
  void Volume(int volume);

  mix::Chunk chunk_;
  int volume_;
};

static int const default_volume = -1;
static std::mutex mutex;
typedef std::weak_ptr<SoundImpl> SoundPtr;
static std::unordered_map<int, SoundPtr> channels;

static bool ChannelEqual(SoundPtr const& a, SoundPtr const& b)
{
  bool equal = true;
  if(a.owner_before(b) || b.owner_before(a))
  {
    equal = false;
  }
  return equal;
}

static void ChannelFinished(int channel)
{
  std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
  if(-1 == std::try_lock(lock))
  {
    (void)channels.erase(channel);
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
  int channel = chunk_.Play(repeats, volume_);
  channels[channel] = shared_from_this();
}

void SoundImpl::Pause(void)
{
  for(auto& channel : channels)
  {
    if(ChannelEqual(channel.second, shared_from_this()))
    {
      Mix_Pause(channel.first);
    }
  }
}

void SoundImpl::Resume(void)
{
  for(auto& channel : channels)
  {
    if(ChannelEqual(channel.second, shared_from_this()))
    {
      Mix_Resume(channel.first);
    }
  }
}

void SoundImpl::Stop(void)
{
  for(auto& channel : channels)
  {
    if(ChannelEqual(channel.second, shared_from_this()))
    {
      (void)Mix_HaltChannel(channel.first);
    }
  }
}

void SoundImpl::Fade(int ms)
{
  for(auto& channel : channels)
  {
    if(ChannelEqual(channel.second, shared_from_this()))
    {
      (void)Mix_FadeOutChannel(channel.first, ms);
    }
  }
}

void SoundImpl::Volume(int volume)
{
  volume_ = volume;
  for(auto& channel : channels)
  {
    if(ChannelEqual(channel.second, shared_from_this()))
    {
      (void)Mix_Volume(channel.first, volume_);
    }
  }
}

Sound::Sound(std::string const& filename)
{
  impl_ = std::make_shared<SoundImpl>(filename);
}

void Sound::Play(int repeats)
{
  thread::Lock lock(mutex);
  impl_->Play(repeats);
}

void Sound::Pause(void) const
{
  thread::Lock lock(mutex);
  impl_->Pause();
}

void Sound::Resume(void) const
{
  thread::Lock lock(mutex);
  impl_->Resume();
}

void Sound::Stop(void) const
{
  thread::Lock lock(mutex);
  impl_->Stop();
}

void Sound::Fade(int ms) const
{
  thread::Lock lock(mutex);
  impl_->Fade(ms);
}

void Sound::Volume(int volume)
{
  thread::Lock lock(mutex);
  impl_->Volume(volume);
}
}
