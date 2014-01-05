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
  ~MusicImpl(void);
  void Pause(void) const;
  void Resume(void);
  void Volume(int volume);

  mix::Library const mix_;
  ffmpeg::Decoder music_;
  int volume_;
};  

static ffmpeg::Decoder* GetCurrentMusic(void)
{
  return static_cast<ffmpeg::Decoder*>(Mix_GetMusicHookData());
}

MusicImpl::~MusicImpl(void)
{
  Mix_HookMusic(nullptr, nullptr);
}

void MusicImpl::Pause(void) const
{
  if(GetCurrentMusic() == &music_)
  {
    Mix_PauseMusic();
  }
}

static void MixCallback(void* music, Uint8* stream, int len)
{
  static_cast<ffmpeg::Decoder*>(music)->Read(stream, len);
}

void MusicImpl::Resume(void)
{
  if(GetCurrentMusic() == &music_)
  {
    Mix_ResumeMusic();
  }
  else
  {
    Mix_HookMusic(MixCallback, static_cast<void*>(&music_));
    (void)Mix_VolumeMusic(volume_);
  }
}

void MusicImpl::Volume(int volume)
{
  volume_ = volume;
  (void)Mix_VolumeMusic(volume_);
}

static int const buffer_size = static_cast<int>(1) << 12;
static int const default_volume = -1;
MusicImpl::MusicImpl(std::string const& filename) : mix_(), volume_(default_volume), music_(filename, buffer_size)
{
}

Music::Music(void)
{
}

Music::Music(std::string const& filename)
{
  impl_ = std::make_shared<MusicImpl>(filename);
}

Music::operator bool(void) const
{
  return bool(impl_);
}

void Music::Pause(void) const
{
  impl_->Pause();
}

void Music::Resume(void)
{
  impl_->Resume();
}

void Music::Volume(int volume)
{
  impl_->Volume(volume);
}
}