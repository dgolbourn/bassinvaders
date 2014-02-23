#include "volume.h"
#include "ffmpeg_exception.h"
#include <sstream>
namespace ffmpeg
{
class VolumeImpl
{
public:
  VolumeImpl(Graph& graph);
  ~VolumeImpl(void);
  void operator()(double volume);
  AVFilterContext* context_;
};

static void FreeContext(AVFilterContext* context)
{
  if(context)
  {
    avfilter_free(context);
  }
}

VolumeImpl::VolumeImpl(Graph& graph)
{
  context_ = nullptr;
  try
  {
    int ret = avfilter_graph_create_filter(&context_, avfilter_get_by_name("volume"), "volume", nullptr, nullptr, graph);
    if(ret < 0)
    {
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error(ret)));
    }
  }
  catch(...)
  {
    FreeContext(context_);
    throw;
  }
}

VolumeImpl::~VolumeImpl(void)
{
  FreeContext(context_);
}

void VolumeImpl::operator()(double volume)
{
  int ret = avfilter_process_command(context_, "volume", std::to_string(volume).c_str(), nullptr, 0, AVFILTER_CMD_FLAG_ONE);
  if(ret < 0)
  {
    BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error(ret)));
  }
}

Volume::Volume(Graph& graph)
{
  impl_ = std::make_shared<VolumeImpl>(graph);
}

void Volume::operator()(double volume)
{
  (*impl_)(volume);
}

Volume::operator AVFilterContext*(void) const
{
  return impl_->context_;
}
}