#include "sound.h"
#include "sound_impl.h"
#include "mix_exception.h"
#include <unordered_map>

namespace audio
{
static bool initialised;
static int const finished = -1;
static std::unordered_map<int, SoundPtr> active_channels;

static void ChannelFinishedCallback(int channel)
{
  auto sound_iter = active_channels.find(channel);
  if(sound_iter != active_channels.end())
  {
    if(auto sound = sound_iter->second.lock())
    {
      sound->channel_ = finished;
      active_channels.erase(sound_iter);
    }
  }
}

void Init(void)
{
  if(!initialised)
  {
    Mix_ChannelFinished(ChannelFinishedCallback);
    initialised = true;
  }
}

SoundImpl::SoundImpl(Mix_Chunk* sound) : sound_(sound), channel_(finished)
{
}

SoundImpl::~SoundImpl(void)
{
  Mix_FreeChunk(sound_);
}

void SoundImpl::Play(int loops, SoundPtr const& impl)
{
  if(channel_ == finished)
  {
    int channel = Mix_PlayChannel(-1, sound_, loops);
    if(channel == -1)
    {
      throw mix::Exception();
    }
    channel_ = channel;
    active_channels[channel] = impl;
  }
}

void SoundImpl::Pause(void) const
{
  if(channel_ != finished)
  {
    Mix_Pause(channel_);
  }
}

void SoundImpl::Resume(void) const
{
  if(channel_ != finished)
  {
    Mix_Resume(channel_);
  }
}

void SoundImpl::Stop(void) const
{
  if(channel_ != finished)
  {
    Mix_HaltChannel(channel_);
  }
}

Sound::Sound(void)
{
}

Sound::Sound(Sound const& other) : impl_(other.impl_)
{
}

Sound::Sound(Sound&& other) : impl_(std::move(other.impl_))
{
}

Sound::~Sound(void)
{
}

Sound& Sound::operator=(Sound other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

void Sound::Play(int loops)
{
  impl_->Play(loops, impl_);
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
}
