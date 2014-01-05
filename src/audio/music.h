#ifndef MUSIC_H_
#define MUSIC_H_

#include <string>
#include <memory>
#include "weak_ptr.h"

namespace audio
{
class Music
{
public:
  Music(void) noexcept;  
  Music(std::string const& filename);
  void Pause(void) const;
  void Resume(void);
  void Volume(int volume);
  explicit operator bool(void) const;
  typedef memory::WeakPtr<Music, class MusicImpl> WeakPtr;
private:
  std::shared_ptr<class MusicImpl> impl_;
  friend WeakPtr;
};
}
#endif