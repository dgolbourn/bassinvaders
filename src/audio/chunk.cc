#include "chunk.h"
#include "SDL_mixer.h"
#include <unordered_map>
#include "mix_exception.h"
#include "mix_library.h"

namespace mix
{
class ChunkImpl
{
public:
  ChunkImpl(std::string const& filename);
  ~ChunkImpl(void);
  mix::Library const mix_;
  Mix_Chunk* chunk_;
};

static std::unordered_map<std::string, std::shared_ptr<ChunkImpl>> chunks;

void Free(std::string const& filename)
{
  auto fileiter = chunks.find(filename);
  if (fileiter != chunks.end())
  {
    chunks.erase(fileiter);
  }
}

void Free(void)
{
  chunks.clear();
}

ChunkImpl::ChunkImpl(std::string const& filename) : mix_()
{
  chunk_ = Mix_LoadWAV(filename.c_str());
  if(!chunk_)
  {
    throw mix::Exception();
  }
}

ChunkImpl::~ChunkImpl(void)
{
  Mix_FreeChunk(chunk_);
}

Chunk::Chunk(std::string const& filename)
{
  auto fileiter = chunks.find(filename);
  if(fileiter != chunks.end())
  {
    impl_ = fileiter->second;
  }
  else
  {
    impl_ = std::shared_ptr<ChunkImpl>(new ChunkImpl(filename));
    chunks[filename] = impl_;
  }
}

int Chunk::Play(int repeats, int volume)
{
  int channel = Mix_PlayChannel(-1, impl_->chunk_, repeats);
  if(channel == -1)
  {
    throw mix::Exception();
  }
  (void)Mix_Volume(channel, volume);
  return channel;
}

Chunk::~Chunk(void)
{
}

Chunk::Chunk(void)
{    
}

Chunk::Chunk(Chunk const& other) : impl_(other.impl_)
{
}

Chunk::Chunk(Chunk&& other) : impl_(std::move(other.impl_))
{
}

Chunk& Chunk::operator=(Chunk other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}