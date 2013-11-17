#ifndef EVENT_H_
#define EVENT_H_

#include "SDL_scancode.h"
#include <unordered_map>
#include "signal.h"
#include "sdl_library.h"

namespace event
{
typedef std::unordered_map<SDL_Scancode, Signal> KeyMap;

extern KeyMap key_map;

extern Signal quit;
extern Signal up;
extern Signal down;
extern Signal left;
extern Signal right;
extern Signal button1;

void Check(void);
void DefaultKeys(void);

class Event
{
public:
  Event(void);
  ~Event(void);
private:
  sdl::Library const sdl_;
};
}
#endif