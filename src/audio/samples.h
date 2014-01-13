#ifndef SAMPLES_H_
#define SAMPLES_H_

#include <stdint.h>
#include <memory>

namespace ffmpeg
{
class Samples
{
public:
  Samples(int samples);
  uint8_t** Data(void);
  void Size(int size) const;
  int Read(uint8_t* data, int size);
  explicit operator bool(void) const;
private:
  std::shared_ptr<class SamplesImpl> impl_;
};
}
#endif