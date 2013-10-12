#ifndef DECODER_H_
#define DECODER_H_

#include <stdint.h>
#include <memory>
#include <string>

namespace audio
{

class Decoder
{
public:
  Decoder(void);
  Decoder(std::string filename);
  Decoder(const Decoder& other);
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