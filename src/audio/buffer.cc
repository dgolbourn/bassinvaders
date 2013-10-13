#include "buffer.h"

#include <queue>

namespace ffmpeg
{

class BufferImpl
{
public:
  std::queue<Samples> queue_;
  uint8_t* data_;
  int total_buffer_size_;
  int data_size_;
  int target_buffer_size_;

  BufferImpl(int size);
  ~BufferImpl(void);

  void Add(Samples samples);
  bool Full(void);
  bool Empty(void);
  int Read(uint8_t* buffer, int size);
};

BufferImpl::BufferImpl(int size)
{
  data_ = nullptr;
  total_buffer_size_ = 0;
  data_size_ = 0;
  target_buffer_size_ = size;
}

BufferImpl::~BufferImpl(void)
{
}

void BufferImpl::Add(Samples samples)
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

bool BufferImpl::Full(void)
{
  return total_buffer_size_ >= target_buffer_size_;
}

bool BufferImpl::Empty(void)
{
  return queue_.empty();
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

  return start_size - size;
}

void Buffer::Add(Samples samples)
{
  impl_->Add(samples);
}

bool Buffer::Full(void)
{
  return impl_->Full();
}

bool Buffer::Empty(void)
{
  return impl_->Empty();
}

int Buffer::Read(uint8_t* buffer, int size)
{
  return impl_->Read(buffer, size);
}

Buffer::Buffer(void)
{
}

Buffer::Buffer(int size)
{
  impl_ = std::shared_ptr<BufferImpl>(new BufferImpl(size));
}

Buffer::Buffer(const Buffer& other)
{
  impl_ = other.impl_;
}

Buffer::Buffer(Buffer&& other)
{
  impl_ = other.impl_;
  other.impl_.reset();
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