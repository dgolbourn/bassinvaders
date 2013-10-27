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

  Sound Load(std::string& filename);
  void Free(std::string& filename);
  void Pause(void);
  void Resume(void);
  void Music(std::string& filename);
  void SoundVolume(int volume);
  void MusicVolume(int volume);
private:
  std::shared_ptr<class MixerImpl> impl_;
};

}
#endif