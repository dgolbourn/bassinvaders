#ifndef FORMAT_H_
#define FORMAT_H_

extern "C"
{
#include "libavformat/avformat.h"
}

#include <string>
#include <memory>

namespace ffmpeg
{

class Format
{
public:
  Format(void);
  Format(std::string const& filename);
  Format(Format const& other);
  Format(Format&& other);
  Format& operator=(Format other);
  ~Format(void);

  AVFormatContext* format(void) const;
  AVStream* audio_stream(void) const;
private:
  std::shared_ptr<class FormatImpl> impl_;
};

}

#endif