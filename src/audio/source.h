#ifndef SOURCE_H_
#define SOURCE_H_
#include <memory>
#include "graph.h"
#include <string>
namespace ffmpeg
{
class Source
{
public:
  Source(void) = default;
  Source(std::string const& filename, Graph& graph);
  operator AVFilterContext*(void) const;
private:
  std::shared_ptr<class SourceImpl> impl_;
};
}
#endif