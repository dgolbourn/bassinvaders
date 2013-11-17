#ifndef MIX_LIBRARY_H_
#define MIX_LIBRARY_H_

#include "sdl_library.h"

namespace mix
{
class Library
{
public:
  Library(void);
  ~Library(void);
private:
  sdl::Library const sdl_;
};
}
#endif