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
  Samples(Samples const& other);
  Samples(Samples&& other);
  Samples& operator=(Samples other);
  ~Samples(void);

  uint8_t**& data(void);
  int size(void);
private:
  std::shared_ptr<class SamplesImpl> impl_;
};

}

#endif