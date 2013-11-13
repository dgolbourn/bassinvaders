#ifndef SOUND_IMPL_H_
#define SOUND_IMPL_H_

#include <memory>
#include "SDL_mixer.h"

namespace audio
{
class SoundImpl
{
public:
  SoundImpl(Mix_Chunk* sound);
  void Play(void) const;

  ~SoundImpl(void);

  Mix_Chunk* sound_;
};
}
#endif