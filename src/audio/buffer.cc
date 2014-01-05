#include "buffer.h"
#include <queue>
#include "thread.h"

namespace ffmpeg
{
class BufferImpl
{
public:
  BufferImpl(int size);
  void Add(Samples const& samples);
  bool Full(void) const;
  explicit operator bool(void) const;
  int Read(uint8_t* buffer, int size);

  std::mutex mutex_;
  std::queue<Samples> queue_;
  uint8_t* data_;
  int data_size_;
  int total_buffer_size_;
  int target_buffer_size_;
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
}

bool BufferImpl::Full(void) const
{
  bool full = total_buffer_size_ >= target_buffer_size_;
  return full;
}

BufferImpl::operator bool(void) const
{
  return !queue_.empty();
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
  return start_size;
}

void Buffer::Add(Samples const& samples)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Add(samples);
}

bool Buffer::Full(void) const
{
  thread::Lock lock(impl_->mutex_);
  return impl_->Full();
}

Buffer::operator bool(void) const
{
  thread::Lock lock(impl_->mutex_);
  return bool(impl_);
}

int Buffer::Read(uint8_t* buffer, int size)
{
  thread::Lock lock(impl_->mutex_);
  return impl_->Read(buffer, size);
}

Buffer::Buffer(void) noexcept
{
}

Buffer::Buffer(int size)
{
  impl_ = std::make_shared<BufferImpl>(size);
}
}