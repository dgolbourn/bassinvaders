#ifndef MIXER_H_
#define MIXER_H_

#include <string>
#include <memory>
#include "sound.h"
#include "signal.h"

namespace audio
{

class Mixer
{
public:
  Mixer(void);
  Mixer(Mixer const& other);
  Mixer(Mixer&& other);
  Mixer& operator=(Mixer other);
  ~Mixer(void);

  Sound Load(std::string const& filename);
  void Free(std::string const& filename);
  void Pause(void) const;
  void Resume(void) const;
  void Music(std::string const& filename);
  void SoundVolume(int volume) const;
  void MusicVolume(int volume) const;
private:
  std::shared_ptr<class MixerImpl> impl_;
};

}
#endif