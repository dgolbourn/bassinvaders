#include "event.h"
#include "SDL_events.h"

namespace event
{

Signal quit;

Signal up;
Signal down;
Signal left;
Signal right;

Signal trigger;

static void QuitEvent(void)
{
  quit.Notify();
}

static void KeydownEvent(SDL_KeyboardEvent& keyboard_event)
{
  switch(keyboard_event.keysym.scancode)
  {
  case SDL_SCANCODE_UP:
  case SDL_SCANCODE_W:
    up.Notify();
    break;
  case SDL_SCANCODE_DOWN:
  case SDL_SCANCODE_S:
    down.Notify();
    break;
  case SDL_SCANCODE_LEFT:
  case SDL_SCANCODE_A:
    left.Notify();
    break;
  case SDL_SCANCODE_RIGHT:
  case SDL_SCANCODE_D:
    right.Notify();
    break;
  case SDL_SCANCODE_SPACE:
    trigger.Notify();
    break;
  default:
    break;
  }
}

void Events(void)
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
    case SDL_QUIT:
      QuitEvent();
      break;
    case SDL_KEYDOWN:
      KeydownEvent(event.key);
      break;
    default:
      break;
    }
  }
}

}