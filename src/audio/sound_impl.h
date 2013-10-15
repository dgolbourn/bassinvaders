#ifndef SOUND_IMPL_H_
#define SOUND_IMPL_H_

#include <memory>
#include "SDL_mixer.h"

namespace audio
{

class SoundImpl
{
public:
  Mix_Chunk* sound_;

  SoundImpl(Mix_Chunk* sound);
  ~SoundImpl(void);

  void Play(void);
};

}

#endif