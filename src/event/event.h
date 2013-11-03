#ifndef EVENT_H_
#define EVENT_H_

#include "signal.h"
#include "SDL_scancode.h"
#include <map>

namespace event
{

typedef std::map<SDL_Scancode, Signal> KeyMap;

extern KeyMap key_map;

extern Signal quit;
extern Signal up;
extern Signal down;
extern Signal left;
extern Signal right;
extern Signal button1;

void Event(void);

void Init(void);
}

#endif