#include "filter.h"
extern "C"
{
#include "libavfilter/avfilter.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/opt.h"
}
#include <sstream>
#include "ffmpeg_exception.h"
#include <iostream>
namespace ffmpeg
{
typedef std::shared_ptr<AVFilterContext> Context;
typedef std::shared_ptr<AVFilterGraph> Graph;
typedef std::shared_ptr<AVFilterInOut> InOutList;

class FilterImpl
{
public:
  FilterImpl(std::string const& description, Format const& format, Codec const& codec);
  void Add(Frame const& frame);
  bool Read(Frame const& frame);
  Graph graph_;
  Context sink_;
  Context source_;
};

static void FreeAVFilterGraph(AVFilterGraph* ptr)
{
  avfilter_graph_free(&ptr);
}

static AVFilterContext* InitSource(Format const& format, Codec const& codec, Graph& graph)
{
  AVRational time_base = format.AudioStream()->time_base;
  std::stringstream args;
  args << "time_base=" << time_base.num << "/" << time_base.den
    << ":sample_rate=" << codec->sample_rate
    << ":sample_fmt=" << av_get_sample_fmt_name(codec->sample_fmt)
    << ":channel_layout=" << std::hex << std::showbase << codec->channel_layout;
  AVFilterContext* source;
  if(avfilter_graph_create_filter(&source, avfilter_get_by_name("abuffer"), "in", args.str().c_str(), nullptr, graph.get()) < 0)
  {
    throw Exception();
  }
  return source;
}

static AVFilterContext* InitSink(Codec const& codec, Graph& graph)
{
  AVFilterContext* sink;
  if(avfilter_graph_create_filter(&sink, avfilter_get_by_name("abuffersink"), "out", nullptr, nullptr, graph.get()) < 0)
  {
    throw Exception();
  }
  AVSampleFormat sample_fmts[] = {codec->sample_fmt, AV_SAMPLE_FMT_NONE};
  int64_t channel_layouts[] = {codec->channel_layout, -1};    
  int sample_rates[] = {codec->sample_rate, -1};
  int ret = 0;
  ret |= av_opt_set_int_list(sink, "sample_fmts", sample_fmts, -1, AV_OPT_SEARCH_CHILDREN);   
  ret |= av_opt_set_int_list(sink, "channel_layouts", channel_layouts, -1, AV_OPT_SEARCH_CHILDREN);      
  ret |= av_opt_set_int_list(sink, "sample_rates", sample_rates, -1, AV_OPT_SEARCH_CHILDREN);   
  if(ret)
  {
    avfilter_free(sink);
    throw Exception();
  }
  return sink;
}

static void FreeAVFilterInOut(AVFilterInOut* ptr)
{
  avfilter_inout_free(&ptr);
}

FilterImpl::FilterImpl(std::string const& description, Format const& format, Codec const& codec)
{
  avfilter_register_all();
  graph_ = Graph(avfilter_graph_alloc(), FreeAVFilterGraph);
  source_ = Context(InitSource(format, codec, graph_), avfilter_free);
  sink_ = Context(InitSink(codec, graph_), avfilter_free);
  
  InOutList::element_type* in;
  InOutList::element_type* out;
  if(avfilter_graph_parse2(graph_.get(), description.c_str(), &in, &out) < 0)
  {
    throw Exception();
  }
  InOutList in_guard(in, FreeAVFilterInOut);
  InOutList out_guard(out, FreeAVFilterInOut);
  if(avfilter_link(source_.get(), 0, in->filter_ctx, in->pad_idx))
  {
    throw Exception();
  }
  if(avfilter_link(out->filter_ctx, out->pad_idx, sink_.get(), 0))
  {
    throw Exception();
  }
  if(avfilter_graph_config(graph_.get(), nullptr) < 0)
  {
    throw Exception();
  }
}

void FilterImpl::Add(Frame const& frame)
{
  if(av_buffersrc_add_frame(source_.get(), frame) < 0)
  {
    throw Exception();
  }
}
  
bool FilterImpl::Read(Frame const& frame)
{
  bool got_frame = true;
  int ret = av_buffersink_get_frame(sink_.get(), frame);
  if((ret == AVERROR(EAGAIN)) || (ret == AVERROR_EOF))
  {
    got_frame = false;
  }
  else if(ret < 0)
  {
    throw Exception();
  }
  return got_frame;
}

Filter::Filter(std::string const& description, Format const& format, Codec const& codec)
{
  impl_ = std::make_shared<FilterImpl>(description, format, codec);
}

void Filter::Add(Frame const& frame)
{
  impl_->Add(frame);
}

bool Filter::Read(Frame const& frame)
{
  return(impl_->Read(frame));
}
}