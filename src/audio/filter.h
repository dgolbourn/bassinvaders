#ifndef FILTER_H_
#define FILTER_H_
#include <memory>
#include <string>
#include "frame.h"
#include "format.h"
#include "codec.h"
namespace ffmpeg
{
class Filter
{
public:
  Filter(void) = default;
  Filter(Format const& format, Codec const& codec);
  void Add(Frame const& frame);
  bool Read(Frame& frame);
  void Volume(double volume);
private:
  std::shared_ptr<class FilterImpl> impl_;
};
}
#endif