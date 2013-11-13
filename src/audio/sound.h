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
  void Play(void) const;

  Sound(Sound const& other);
  Sound(Sound&& other);
  Sound& operator=(Sound other);
  ~Sound(void);
private:
  std::shared_ptr<class SoundImpl> impl_;
};
}
#endif