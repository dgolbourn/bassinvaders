#ifndef MUTEX_H_
#define MUTEX_H_
#include <mutex>
#include <memory>
#include "SDL_mutex.h"
namespace sdl
{
class Mutex
{
public:
  Mutex(void);
  void lock(void);
  void unlock(void);
private:
  std::shared_ptr<SDL_mutex> mutex_;
};

typedef std::lock_guard<Mutex> Lock;
}
#endif