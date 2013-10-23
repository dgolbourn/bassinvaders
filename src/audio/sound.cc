#include "sound.h"
#include "sound_impl.h"
#include "mix_exception.h"

namespace audio
{

SoundImpl::SoundImpl(Mix_Chunk* sound)
{
  sound_ = sound;
}

SoundImpl::~SoundImpl(void)
{
  Mix_FreeChunk(sound_);
}

void SoundImpl::Play(void)
{
  if(Mix_PlayChannel(-1, sound_, 0) == -1)
  {
    throw mix::Exception();
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

void Sound::Play(void)
{
  impl_->Play();
}

}
