#include <map>
#include "music.h"
#include "SDL_mixer.h"
#include "mix_library.h"
#include "decoder.h"

namespace audio
{
class MusicImpl
{  
public:
  MusicImpl(std::string const& filename);
  void Pause(void) const;
  void Resume(void) const;
  void Volume(int volume) const;

  ffmpeg::Decoder music_;
  mix::Library const mix_;
};  

void MusicImpl::Pause(void) const
{
  Mix_PauseMusic();
}

void MusicImpl::Resume(void) const
{
  Mix_ResumeMusic();
}

void MusicImpl::Volume(int volume) const
{
  Mix_VolumeMusic(volume);
}

static void MixCallback(void* music, Uint8* stream, int len)
{
  static_cast<ffmpeg::Decoder*>(music)->Read(stream, len);
}

MusicImpl::MusicImpl(std::string const& filename) : mix_()
{
  int const buffer_size = static_cast<int>(1) << 12;
  music_ = ffmpeg::Decoder(filename, buffer_size);
  Mix_HookMusic(MixCallback, static_cast<void*>(&music_));
  Mix_PauseMusic();
}

Music::Music(void)
{
}

Music::Music(std::string const& filename) : impl_(new MusicImpl(filename))
{
}

Music::Music(Music const& other) : impl_(other.impl_)
{
}

Music::Music(Music&& other) : impl_(std::move(other.impl_))
{
}

Music::~Music(void)
{
}

Music& Music::operator=(Music other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

void Music::Pause(void) const
{
  impl_->Pause();
}

void Music::Resume(void) const
{
  impl_->Resume();
}

void Music::Volume(int volume) const
{
  impl_->Volume(volume);
}
}