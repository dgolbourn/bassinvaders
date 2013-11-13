#ifndef BUFFER_H_
#define BUFFER_H_

#include <cstdint>
#include "samples.h"

namespace ffmpeg
{
class Buffer
{
public:
  Buffer(void);
  Buffer(int size);
  void Add(Samples const& samples);
  bool Full(void);
  int Read(uint8_t* buffer, int size);
  explicit operator bool(void) const;

  Buffer(Buffer const& other);
  Buffer(Buffer&& other);
  Buffer& operator=(Buffer other);
  ~Buffer(void);
private:
  std::shared_ptr<class BufferImpl> impl_;
};
}
#endif