#ifndef SAMPLES_H_
#define SAMPLES_H_

#include <stdint.h>
#include <memory>
#include "frame.h"

namespace ffmpeg
{
class Samples
{
public:
  Samples(Frame const& frame);
  int Read(uint8_t* data, int size);
  explicit operator bool(void) const;
private:
  std::shared_ptr<class SamplesImpl> impl_;
};
}
#endif