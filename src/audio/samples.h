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
  Samples(void) = default;
  uint8_t** data(void);
  int size(void) const;
  void size(int size) const;
private:
  std::shared_ptr<class SamplesImpl> impl_;
};
}
#endif