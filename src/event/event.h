#ifndef EVENT_H_
#define EVENT_H_

#include "signal.h"
#include "SDL_scancode.h"
#include <map>

namespace event
{
enum Action
{
  UP,
  DOWN,
  LEFT,
  RIGHT,
  BUTTON1
};

extern std::map<SDL_Scancode, Action> key_map;

extern Signal quit;

extern Signal up;
extern Signal down;
extern Signal left;
extern Signal right;
extern Signal button1;

void Events(void);

void Init(void);
}

#endif