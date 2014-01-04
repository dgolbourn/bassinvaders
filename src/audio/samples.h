#ifndef SAMPLES_H_
#define SAMPLES_H_

extern "C"
{
#include "libavutil/samplefmt.h"
}
#include <stdint.h>
#include <memory>

namespace ffmpeg
{
class Samples
{
public:
  Samples(uint8_t** data, int size);
  Samples(void);
  uint8_t**& data(void) const;
  int size(void) const;
private:
  std::shared_ptr<class SamplesImpl> impl_;
};
}
#endif