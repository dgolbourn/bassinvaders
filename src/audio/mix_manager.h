#ifndef MIX_MANAGER_H_
#define MIX_MANAGER_H_

#include "sdl_manager.h"

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