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
  Decoder(void);
  Decoder(std::string const& filename, int buffer_size);
  void Read(uint8_t* buffer, int size);
  explicit operator bool(void) const;

  Decoder(Decoder const& other);
  Decoder(Decoder&& other);
  Decoder& operator=(Decoder other);
  ~Decoder(void);
private:
  std::shared_ptr<class DecoderImpl> impl_;
};
}
#endif