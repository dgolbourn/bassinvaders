#ifndef SOUND_H_
#define SOUND_H_

#include <memory>

namespace audio
{

class Sound
{
  friend class MixerImpl;
public:
  Sound(void);
  Sound(Sound const& other);
  Sound(Sound&& other);
  Sound& operator=(Sound other);
  ~Sound(void);

  void Play(void) const;
private:
  std::shared_ptr<class SoundImpl> impl_;
};

}

#endif