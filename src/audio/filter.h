#ifndef FILTER_H_
#define FILTER_H_
#include <memory>
#include <string>
#include "frame.h"
namespace ffmpeg
{
class Filter
{
public:
  Filter(void) = default;
  Filter(std::string const& filename);
  void Add(Frame const& frame);
  bool Read(Frame& frame);
  void Volume(double volume);
private:
  std::shared_ptr<class FilterImpl> impl_;
};
}
#endif