#include "buffer.h"
#include <queue>
#include "thread.h"

namespace ffmpeg
{
class BufferImpl
{
public:
  void Add(Samples const& samples);
  int Read(uint8_t* buffer, int size);
  std::queue<Samples> queue_;
};

void BufferImpl::Add(Samples const& samples)
{
  queue_.push(samples);
}

int BufferImpl::Read(uint8_t* buffer, int size)
{
  int start_size = size;
  while(size)
  {
    if(!queue_.empty())
    {
      size -= queue_.front().Read(buffer, size);
      if(!queue_.front())
      {
        queue_.pop();
      }
    }
    else
    {
      break;
    }
  }
  start_size -= size;
  return start_size;
}

void Buffer::Add(Samples const& samples)
{
  impl_->Add(samples);
}

Buffer::operator bool(void) const
{
  return !impl_->queue_.empty();
}

int Buffer::Read(uint8_t* buffer, int size)
{
  return impl_->Read(buffer, size);
}

Buffer::Buffer(void)
{
  impl_ = std::make_shared<BufferImpl>();
}
}