#include "graph.h"
#include "ffmpeg_exception.h"
namespace ffmpeg
{
class GraphImpl
{
public:
  GraphImpl(void);
  ~GraphImpl(void);
  void operator()(void);
  AVFilterGraph* graph_;
};

GraphImpl::GraphImpl(void)
{
  graph_ = nullptr;
  try
  {
    graph_ = avfilter_graph_alloc();
    if(!graph_)
    {
      throw Exception();
    }
  }
  catch(...)
  {
    avfilter_graph_free(&graph_);
    throw;
  }
}

GraphImpl::~GraphImpl(void)
{
  avfilter_graph_free(&graph_);
}

void GraphImpl::operator()(void)
{
  if(avfilter_graph_config(graph_, nullptr) < 0)
  {
    throw Exception();
  }
}

Graph::Graph(void)
{
  impl_ = std::make_shared<GraphImpl>();
}

void Graph::operator()(void)
{
  (*impl_)();
}

Graph::operator AVFilterGraph*(void) const
{
  return impl_->graph_;
}
}