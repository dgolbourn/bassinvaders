#ifndef SOUND_IMPL_H_
#define SOUND_IMPL_H_

#include <memory>
#include "SDL_mixer.h"

namespace audio
{
typedef std::weak_ptr<class SoundImpl> SoundPtr;

class SoundImpl
{
public:
  SoundImpl(Mix_Chunk* sound);
  void Play(int loops, SoundPtr const& impl);
  void Pause(void) const;
  void Resume(void) const;
  void Stop(void) const;

  ~SoundImpl(void);

  Mix_Chunk* sound_;
  int channel_;
};

void Init(void);
}
#endif