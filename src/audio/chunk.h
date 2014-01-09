#ifndef CHUNK_H_
#define CHUNK_H_

#include <memory>
#include <string>

namespace mix
{
class Chunk
{
public:
  Chunk(void) = default;
  Chunk(std::string const& filename);
  int Play(int repeats, int volume);
private:
  std::shared_ptr<class ChunkImpl> impl_;
};

void Free(std::string const& filename);
void Free(void);
}
#endif