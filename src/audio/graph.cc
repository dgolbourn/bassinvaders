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
      BOOST_THROW_EXCEPTION(Exception());
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
  int ret = avfilter_graph_config(graph_, nullptr);
  if(ret < 0)
  {
    BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error(ret)));
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