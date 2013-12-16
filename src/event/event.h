#ifndef EVENT_H_
#define EVENT_H_

#include "SDL_scancode.h"
#include <unordered_map>
#include <utility>
#include "signal.h"
#include "sdl_library.h"

namespace event
{
typedef std::pair<Signal, Signal> Switch;
typedef std::unordered_map<SDL_Scancode, Switch> KeyMap;

extern KeyMap key_map;

extern Signal quit;
extern Switch up;
extern Switch down;
extern Switch left;
extern Switch right;
extern Switch button1;
extern Switch pause;

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