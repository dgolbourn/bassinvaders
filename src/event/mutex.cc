#include "mutex.h"
#include "sdl_exception.h"

namespace sdl
{
static SDL_mutex* CreateMutex(void)
{
  SDL_mutex* mutex = SDL_CreateMutex();
  if(!mutex)
  {
    throw Exception();
  }
  return mutex;
}

void Mutex::lock(void)
{
  if(SDL_LockMutex(mutex_.get()))
  {
    throw Exception();
  }
}

void Mutex::unlock(void)
{
  if(SDL_UnlockMutex(mutex_.get()))
  {
    throw Exception();
  }
}

Mutex::Mutex(void) : mutex_(CreateMutex(), SDL_DestroyMutex)
{
}
};