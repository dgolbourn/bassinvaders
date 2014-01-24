#include "chunk.h"
#include "SDL_mixer.h"
#include <unordered_map>
#include "mix_exception.h"
#include "mix_library.h"
#include "thread.h"

namespace mix
{
class ChunkImpl
{
public:
  ChunkImpl(std::string const& filename);
  ~ChunkImpl(void);
  Library const mix_;
  Mix_Chunk* chunk_;
};

static std::mutex mutex;
static std::unordered_map<std::string, std::shared_ptr<ChunkImpl>> chunks;

void Free(void)
{
  thread::Lock lock(mutex);
  chunks.clear();
}

ChunkImpl::ChunkImpl(std::string const& filename) : mix_()
{
  chunk_ = Mix_LoadWAV(filename.c_str());
  if(!chunk_)
  {
    throw Exception();
  }
}

ChunkImpl::~ChunkImpl(void)
{
  Mix_FreeChunk(chunk_);
}

Chunk::Chunk(std::string const& filename)
{
  thread::Lock lock(mutex);
  auto fileiter = chunks.find(filename);
  if(fileiter != chunks.end())
  {
    impl_ = fileiter->second;
  }
  else
  {
    impl_ = std::make_shared<ChunkImpl>(filename);
    chunks[filename] = impl_;
  }
}

int Chunk::Play(int repeats, int volume)
{
  int channel = Mix_PlayChannel(-1, impl_->chunk_, repeats);
  if(channel == -1)
  {
    throw Exception();
  }
  (void)Mix_Volume(channel, volume);
  return channel;
}
}