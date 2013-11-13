#include "buffer.h"
#include <queue>
#include <mutex>

namespace ffmpeg
{
class BufferImpl
{
public:
  BufferImpl(int size);
  void Add(Samples const& samples);
  bool Full(void);
  explicit operator bool(void) const;
  int Read(uint8_t* buffer, int size);

  std::queue<Samples> queue_;
  uint8_t* data_;
  int data_size_;
  int total_buffer_size_;
  int target_buffer_size_;
  std::mutex mutex_;
};

BufferImpl::BufferImpl(int size)
{
  data_ = nullptr;
  total_buffer_size_ = 0;
  data_size_ = 0;
  if(size < 1)
  {
    size = 1;
  }
  target_buffer_size_ = size;
}

void BufferImpl::Add(Samples const& samples)
{
  mutex_.lock();
  bool empty_flag = false;
  if(queue_.empty())
  {
    empty_flag = true;
  }
  
  total_buffer_size_ += samples.size();
  queue_.push(samples);

  if(empty_flag)
  {
    data_size_ = queue_.front().size();
    data_ = queue_.front().data()[0];
  }
  mutex_.unlock();
}

bool BufferImpl::Full(void)
{
  mutex_.lock();
  bool full = total_buffer_size_ >= target_buffer_size_;
  mutex_.unlock();
  return full;
}

BufferImpl::operator bool(void) const
{
  return !queue_.empty();
}

int BufferImpl::Read(uint8_t* buffer, int size)
{
  mutex_.lock();
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
        total_buffer_size_ -= size;
        size = 0;
      }
      else
      {
        memcpy(buffer, data_, data_size_);
        buffer += data_size_;
        total_buffer_size_ -= data_size_;
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
  mutex_.unlock();
  return start_size;
}

void Buffer::Add(Samples const& samples)
{
  impl_->Add(samples);
}

bool Buffer::Full(void)
{
  return impl_->Full();
}

Buffer::operator bool(void) const
{
  return bool(impl_);
}

int Buffer::Read(uint8_t* buffer, int size)
{
  return impl_->Read(buffer, size);
}

Buffer::Buffer(void)
{
}

Buffer::Buffer(int size) : impl_((new BufferImpl(size)))
{
}

Buffer::Buffer(Buffer const& other) : impl_(other.impl_)
{
}

Buffer::Buffer(Buffer&& other) : impl_(std::move(other.impl_))
{
}

Buffer& Buffer::operator=(Buffer other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

Buffer::~Buffer(void)
{
}

}