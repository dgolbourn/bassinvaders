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
  Format(std::string filename);
  Format(Format const& other);
  Format(Format&& other);
  Format& operator=(Format other);
  ~Format(void);

  AVFormatContext* format(void);
  AVStream* audio_stream(void);
private:
  std::shared_ptr<class FormatImpl> impl_;
};

}

#endif