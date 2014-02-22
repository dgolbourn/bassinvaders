#include "Sink.h"
extern "C"
{
#include "libavfilter/buffersink.h"
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
}
#include "audio_format.h"
#include "ffmpeg_exception.h"
#include <sstream>
namespace ffmpeg
{
class SinkImpl
{
public:
  SinkImpl(Graph& graph);
  ~SinkImpl(void);
  bool operator()(Frame& frame);
  AVFilterContext* context_;
};

static void FreeContext(AVFilterContext* context)
{
  if(context)
  {
    avfilter_free(context);
  }
}

SinkImpl::SinkImpl(Graph& graph)
{
  context_ = nullptr;
  try
  {
    if(avfilter_graph_create_filter(&context_, avfilter_get_by_name("abuffersink"), "out", nullptr, nullptr, graph) < 0)
    {
      throw Exception();
    }
    static AVSampleFormat const sample_fmts[] = {FFMPEG_FORMAT, AV_SAMPLE_FMT_NONE};
    static int64_t const channel_layouts[] = {FFMPEG_CHANNEL_LAYOUT, -1};
    static int const sample_rates[] = {FFMPEG_SAMPLE_RATE, -1};
    int ret = 0;
    ret |= av_opt_set_int_list(context_, "sample_fmts", sample_fmts, -1, AV_OPT_SEARCH_CHILDREN);
    ret |= av_opt_set_int_list(context_, "channel_layouts", channel_layouts, -1, AV_OPT_SEARCH_CHILDREN);
    ret |= av_opt_set_int_list(context_, "sample_rates", sample_rates, -1, AV_OPT_SEARCH_CHILDREN);
    if (ret)
    {
      throw Exception();
    }
  }
  catch(...)
  {
    FreeContext(context_);
    throw;
  }
}

SinkImpl::~SinkImpl(void)
{
  FreeContext(context_);
}

bool SinkImpl::operator()(Frame& frame)
{
  bool got_frame = true;
  int ret = av_buffersink_get_frame(context_, frame);
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

Sink::Sink(Graph& graph)
{
  impl_ = std::make_shared<SinkImpl>(graph);
}

bool Sink::operator()(Frame& frame)
{
  return (*impl_)(frame);
}

Sink::operator AVFilterContext*(void) const
{
  return impl_->context_;
}
}