#ifndef FORMAT_H_
#define FORMAT_H_

extern "C"
{
#include "libavformat/avformat.h"
}
#include <string>
#include <memory>
#include "packet.h"

namespace ffmpeg
{
class Format
{
public:
  Format(void) = default;
  Format(std::string const& filename);
  operator AVFormatContext*(void) const;
  AVFormatContext* operator->(void) const;
  bool Read(Packet& packet);
private:
  std::shared_ptr<AVFormatContext> impl_;
};
}
#endif