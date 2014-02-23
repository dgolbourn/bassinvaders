#include "source.h"
#include <sstream>
#include "ffmpeg_exception.h"
namespace ffmpeg
{
class SourceImpl
{
public:
  SourceImpl(std::string const& filename, Graph& graph);
  ~SourceImpl(void);
  AVFilterContext* context_;
};

static void FreeContext(AVFilterContext* context)
{
  if(context)
  {
    avfilter_free(context);
  }
}

SourceImpl::SourceImpl(std::string const& filename, Graph& graph)
{
  context_ = nullptr;
  try
  {
    std::stringstream args;
    args << "filename='" << filename << "'";
    int ret = avfilter_graph_create_filter(&context_, avfilter_get_by_name("amovie"), "in", args.str().c_str(), nullptr, graph);
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

SourceImpl::~SourceImpl(void)
{
  FreeContext(context_);
}

Source::Source(std::string const& filename, Graph& graph)
{
  impl_ = std::make_shared<SourceImpl>(filename, graph);
}

Source::operator AVFilterContext*(void) const
{
  return impl_->context_;
}
}