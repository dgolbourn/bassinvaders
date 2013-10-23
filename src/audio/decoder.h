#ifndef DECODER_H_
#define DECODER_H_

#include <stdint.h>
#include <memory>
#include <string>

namespace audio
{
#define BUFFER_SIZE_DEFAULT (1 << 20)

class Decoder
{
public:
  Decoder(void);
  Decoder(std::string filename, int buffer_size = BUFFER_SIZE_DEFAULT);
  Decoder(Decoder const& other);
  Decoder(Decoder&& other);
  Decoder& operator=(Decoder other);
  ~Decoder(void);

  void Read(uint8_t* buffer, int size);
  bool Empty(void);
private:
  std::shared_ptr<class DecoderImpl> impl_;
};

}

#endif