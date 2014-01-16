#ifndef RESAMPLER_H_
#define RESAMPLER_H_

#include <memory>
#include "codec.h"
#include "samples.h"
#include "frame.h"

namespace ffmpeg
{
class Resampler
{
public:
  Resampler(void) = default;
  Resampler(Codec const& codec);
  Samples operator()(Codec const& codec, Frame const& frame);
private:
  std::shared_ptr<class ResamplerImpl> impl_;
};
}
#endif