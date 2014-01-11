#include "buffer.h"
#include <queue>
#include "thread.h"

namespace ffmpeg
{
class BufferImpl
{
public:
  BufferImpl(void);
  void Add(Samples const& samples);
  int Read(uint8_t* buffer, int size);

  std::mutex mutex_;
  std::queue<Samples> queue_;
  uint8_t* data_;
  int data_size_;
  int buffer_size_;
};

BufferImpl::BufferImpl(void)
{
  data_ = nullptr;
  buffer_size_ = 0;
  data_size_ = 0;
}

void BufferImpl::Add(Samples const& samples)
{
  bool empty_flag = false;
  if(queue_.empty())
  {
    empty_flag = true;
  }
  
  buffer_size_ += samples.size();
  queue_.push(samples);

  if(empty_flag)
  {
    data_size_ = queue_.front().size();
    data_ = queue_.front().data()[0];
  }
}

int BufferImpl::Read(uint8_t* buffer, int size)
{
  int start_size = size;
  while(size)
  {
    if(!queue_.empty())
    {
      if(size < data_size_)
      {
        memcpy(buffer, data_, size);
        data_ += size;
        data_size_ -= size;
        buffer_size_ -= size;
        size = 0;
      }
      else
      {
        memcpy(buffer, data_, data_size_);
        buffer += data_size_;
        buffer_size_ -= data_size_;
        size -= data_size_;
        queue_.pop();
        if(!queue_.empty())
        {
          data_ = queue_.front().data()[0];
          data_size_ = queue_.front().size();
        }
        else
        {
          break;
        }
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
  thread::Lock lock(impl_->mutex_);
  impl_->Add(samples);
}

Buffer::operator bool(void) const
{
  return impl_->buffer_size_ > 0;
}

int Buffer::Read(uint8_t* buffer, int size)
{
  thread::Lock lock(impl_->mutex_);
  return impl_->Read(buffer, size);
}

Buffer::Buffer(void)
{
  impl_ = std::make_shared<BufferImpl>();
}
}