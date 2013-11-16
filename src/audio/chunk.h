#ifndef CHUNK_H_
#define CHUNK_H_

#include <memory>
#include <string>

namespace mix
{
class Chunk
{
public:
  Chunk(void);
  Chunk(std::string const& filename);
  int Play(int repeats, int volume);

  Chunk(Chunk const& other);
  Chunk(Chunk&& other);
  Chunk& operator=(Chunk other);
  ~Chunk(void);
private:
  std::shared_ptr<class ChunkImpl> impl_;
};

void Free(std::string const& filename);
void Free(void);
}
#endif