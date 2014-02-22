#ifndef GRAPH_H_
#define GRAPH_H_
extern "C"
{
#include "libavfilter/avfilter.h"
}
#include<memory>
namespace ffmpeg
{
class Graph
{
public:
  Graph(void);
  void operator()(void);
  operator AVFilterGraph*(void) const;
private:
  std::shared_ptr<class GraphImpl> impl_;
};
}
#endif