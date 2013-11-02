#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>
#include "samples.h"

namespace ffmpeg
{

class Buffer
{
public:
  Buffer(void);
  Buffer(int size);
  Buffer(Buffer const& other);
  Buffer(Buffer&& other);
  Buffer& operator=(Buffer other);
  ~Buffer(void);

  void Add(Samples const& samples);
  bool Full(void);
  bool Empty(void);
  int Read(uint8_t* buffer, int size);
private:
  std::shared_ptr<class BufferImpl> impl_;
};

}

#endif