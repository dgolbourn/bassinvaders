#include <map>
#include "music.h"
#include "SDL_mixer.h"
#include "mix_library.h"
#include "decoder.h"
#include "cstd_exception.h"
#include <atomic>
#include "boost/throw_exception.hpp"
namespace audio
{
class MusicImpl : public std::enable_shared_from_this<MusicImpl>
{  
public:
  MusicImpl(std::string const& filename);
  void Play(void);
  void Stop(void);
  void Pause(void);
  void Resume(void);
  void Volume(double volume);

  mix::Library const mix_;
  ffmpeg::Decoder music_;
  std::atomic<bool> playing_;
};  

static std::weak_ptr<MusicImpl> music_ptr;

static void MixCallback(void*, Uint8* stream, int len)
{
  auto music = music_ptr.lock();
  if(music && music->playing_)
  {
    int read = music->music_.Read(stream, len);
    stream += read;
    len -= read;
  }
  if(!memset(stream, 0, len))
  {
    BOOST_THROW_EXCEPTION(cstd::Exception());
  }
}

static void InitMusic(void)
{
  static bool initialised;
  if(!initialised)
  {
    Mix_HookMusic(MixCallback, nullptr);
    initialised = true;
  }
}

void MusicImpl::Pause(void)
{
  playing_ = false;
}

void MusicImpl::Stop(void)
{
  playing_ = false;
  if(music_ptr.lock() == shared_from_this())
  {
    music_ptr.reset();
  }
}

void MusicImpl::Resume(void)
{
  playing_ = true;
}

void MusicImpl::Play(void)
{
  playing_ = true;
  music_ptr = shared_from_this();
}

void MusicImpl::Volume(double volume)
{
  music_.Volume(volume);
}

MusicImpl::MusicImpl(std::string const& filename) : music_(filename), playing_(false)
{
  InitMusic();
}

Music::Music(std::string const& filename)
{
  impl_ = std::make_shared<MusicImpl>(filename);
}

Music::operator bool(void) const
{
  return bool(impl_);
}

void Music::Pause(void)
{
  impl_->Pause();
}

void Music::Resume(void)
{
  impl_->Resume();
}

void Music::Stop(void)
{
  impl_->Stop();
}

void Music::Play(void)
{
  impl_->Play();
}

void Music::Volume(double volume)
{
  impl_->Volume(volume);
}
}