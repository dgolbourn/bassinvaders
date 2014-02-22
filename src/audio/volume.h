#ifndef VOLUME_H_
#define VOLUME_H_
#include<memory>
#include "graph.h"
namespace ffmpeg
{
class Volume
{
public:
  Volume(void) = default;
  Volume(Graph& graph);
  void operator()(double volume);
  operator AVFilterContext*(void) const;
private:
  std::shared_ptr<class VolumeImpl> impl_;
};
}
#endif