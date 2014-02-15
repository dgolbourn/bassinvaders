#ifndef DECODER_H_
#define DECODER_H_

#include <stdint.h>
#include <memory>
#include <string>

namespace ffmpeg
{
class Decoder
{
public:
  Decoder(std::string const& filename);
  int Read(uint8_t* buffer, int size);
  void Volume(double volume);
private:
  std::shared_ptr<class DecoderImpl> impl_;
};
}
#endif