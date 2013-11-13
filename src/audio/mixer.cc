#include <map>
#include "mixer.h"
#include "SDL_mixer.h"
#include "mix_exception.h"
#include "mix_manager.h"
#include "sound_impl.h"
#include "decoder.h"

namespace audio
{
class MixerImpl
{  
public:
  MixerImpl(void);
  Sound Load(std::string const& filename);
  void Free(std::string const& filename);
  void Pause(void) const;
  void Resume(void) const;
  void Music(std::string const& filename);
  void SoundVolume(int volume) const;
  void MusicVolume(int volume) const;

  ~MixerImpl(void);

  std::map<std::string, Sound> sounds_;
  ffmpeg::Decoder music_;
};

MixerImpl::MixerImpl(void)
{
  mix::Init();
}

MixerImpl::~MixerImpl(void)
{
  mix::Quit();
}  

Sound MixerImpl::Load(std::string const& filename)
{
  Sound sound;

  auto fileiter = sounds_.find(filename);
  if(fileiter != sounds_.end())
  {
    sound = fileiter->second;
  }
  else
  {
    Mix_Chunk* chunk = Mix_LoadWAV(filename.c_str());
    if(!chunk)
    {
      throw mix::Exception();
    }

    sound.impl_ = std::shared_ptr<SoundImpl>(new SoundImpl(chunk));
    sounds_[filename] = sound;
  }

  return sound;
}

void MixerImpl::Free(std::string const& filename)
{
  auto fileiter = sounds_.find(filename);
  if(fileiter != sounds_.end())
  {
    sounds_.erase(fileiter);
  }    
}

void MixerImpl::Pause(void) const
{
  Mix_Pause(-1);
  Mix_PauseMusic();
}

void MixerImpl::Resume(void) const
{
  Mix_Resume(-1);
  Mix_ResumeMusic();
}

void MixerImpl::SoundVolume(int volume) const
{
  Mix_Volume(-1, volume);
}

void MixerImpl::MusicVolume(int volume) const
{
  Mix_VolumeMusic(volume);
}

static void MixCallback(void* music, Uint8* stream, int len)
{
  static_cast<ffmpeg::Decoder*>(music)->Read(stream, len);
}

void MixerImpl::Music(std::string const& filename)
{
  int const buffer_size = 1 << 12;
  music_ = ffmpeg::Decoder(filename, buffer_size);
  Mix_HookMusic(MixCallback, static_cast<void*>(&music_));
  Mix_PauseMusic();
}

Mixer::Mixer(void) : impl_(new MixerImpl())
{
}

Mixer::Mixer(Mixer const& other) : impl_(other.impl_)
{
}

Mixer::Mixer(Mixer&& other) : impl_(std::move(other.impl_))
{
}

Mixer::~Mixer(void)
{
}

Mixer& Mixer::operator=(Mixer other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

Sound Mixer::Load(std::string const& filename)
{
  return impl_->Load(filename);
}

void Mixer::Free(std::string const& filename)
{
  impl_->Free(filename);
}

void Mixer::Pause(void) const
{
  impl_->Pause();
}

void Mixer::Resume(void) const
{
  impl_->Resume();
}

void Mixer::Music(std::string const& filename)
{
  impl_->Music(filename);
}

void Mixer::SoundVolume(int volume) const
{
  impl_->SoundVolume(volume);
}

void Mixer::MusicVolume(int volume) const
{
  impl_->MusicVolume(volume);
}

}