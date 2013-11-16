#ifndef MIXER_H_
#define MIXER_H_

#include <string>
#include <memory>

namespace audio
{
class Music
{
public:
  Music(void);  
  Music(std::string const& filename);
  void Pause(void) const;
  void Resume(void) const;
  void Volume(int volume) const;

  Music(Music const& other);
  Music(Music&& other);
  Music& operator=(Music other);
  ~Music(void);
private:
  std::shared_ptr<class MusicImpl> impl_;
};
}
#endif