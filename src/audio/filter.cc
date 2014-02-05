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
#include "audio_format.h"

namespace ffmpeg
{
typedef std::shared_ptr<AVFilterContext> Context;
typedef std::shared_ptr<AVFilterGraph> Graph;

class FilterImpl
{
public:
  FilterImpl(Format const& format, Codec const& codec);
  void Add(Frame const& frame);
  bool Read(Frame& frame);
  void Volume(double volume);
  Graph graph_;
  Context sink_;
  Context source_;
  Context volume_;
  Context resample_;
};

static void FreeAVFilterGraph(AVFilterGraph* ptr)
{
  avfilter_graph_free(&ptr);
}

static AVFilterContext* InitSource(Format const& format, Codec const& codec, Graph& graph)
{
  AVRational time_base = format->streams[codec.Stream()]->time_base;
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

static AVFilterContext* InitSink(Graph& graph)
{
  AVFilterContext* sink;
  if(avfilter_graph_create_filter(&sink, avfilter_get_by_name("abuffersink"), "out", nullptr, nullptr, graph.get()) < 0)
  {
    throw Exception();
  }
  static AVSampleFormat const sample_fmts[] = { FFMPEG_FORMAT, AV_SAMPLE_FMT_NONE };
  static int64_t const channel_layouts[] = {FFMPEG_CHANNEL_LAYOUT, -1};    
  static int const sample_rates[] = {FFMPEG_SAMPLE_RATE, -1};
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

static AVFilterContext* InitVolume(Graph& graph)
{
  AVFilterContext* volume;
  if(avfilter_graph_create_filter(&volume, avfilter_get_by_name("volume"), "volume", nullptr, nullptr, graph.get()) < 0)
  {
    throw Exception();
  }
  return volume;
}

static AVFilterContext* InitResample(Graph& graph)
{
  std::stringstream args;
  args << FFMPEG_SAMPLE_RATE
    << ":out_sample_fmt=" << av_get_sample_fmt_name(FFMPEG_FORMAT)
    << ":out_channel_layout=" << std::hex << std::showbase << FFMPEG_CHANNEL_LAYOUT;
  AVFilterContext* resample;
  if(avfilter_graph_create_filter(&resample, avfilter_get_by_name("aresample"), "resample", args.str().c_str(), nullptr, graph.get()) < 0)
  {
    throw Exception();
  }
  return resample;
}

static void FreeAVFilterInOut(AVFilterInOut* ptr)
{
  avfilter_inout_free(&ptr);
}

FilterImpl::FilterImpl(Format const& format, Codec const& codec)
{
  avfilter_register_all();
  graph_ = Graph(avfilter_graph_alloc(), FreeAVFilterGraph);
  source_ = Context(InitSource(format, codec, graph_), avfilter_free);
  sink_ = Context(InitSink(graph_), avfilter_free);
  volume_ = Context(InitVolume(graph_), avfilter_free);
  resample_ = Context(InitResample(graph_), avfilter_free);

  if(avfilter_link(source_.get(), 0, resample_.get(), 0))
  {
    throw Exception();
  }
  if(avfilter_link(resample_.get(), 0, volume_.get(), 0))
  {
    throw Exception();
  }
  if(avfilter_link(volume_.get(), 0, sink_.get(), 0))
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
  
bool FilterImpl::Read(Frame& frame)
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
  if(got_frame)
  {
    frame.Seek();
  }
  return got_frame;
}

void FilterImpl::Volume(double volume)
{
  std::stringstream args;
  args << volume;
  if(avfilter_graph_send_command(graph_.get(), "volume", "volume", args.str().c_str(), nullptr, 0, AVFILTER_CMD_FLAG_ONE) < 0)
  {
    throw Exception();
  }
}

Filter::Filter(Format const& format, Codec const& codec)
{
  impl_ = std::make_shared<FilterImpl>(format, codec);
}

void Filter::Add(Frame const& frame)
{
  impl_->Add(frame);
}

bool Filter::Read(Frame& frame)
{
  return impl_->Read(frame);
}

void Filter::Volume(double volume)
{
  impl_->Volume(volume);
}
}