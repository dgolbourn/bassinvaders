#ifndef SINK_H_
#define SINK_H_
#include<memory>
#include "graph.h"
#include "frame.h"
namespace ffmpeg
{
class Sink
{
public:
  Sink(void) = default;
  Sink(Graph& graph);
  bool operator()(Frame& frame);
  operator AVFilterContext*(void) const;
private:
  std::shared_ptr<class SinkImpl> impl_;
};
}
#endif