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
  quit.Emit();
}

static void KeydownEvent(SDL_KeyboardEvent& keyboard_event)
{
  switch(keyboard_event.keysym.scancode)
  {
  case SDL_SCANCODE_UP:
  case SDL_SCANCODE_W:
    up.Emit();
    break;
  case SDL_SCANCODE_DOWN:
  case SDL_SCANCODE_S:
    down.Emit();
    break;
  case SDL_SCANCODE_LEFT:
  case SDL_SCANCODE_A:
    left.Emit();
    break;
  case SDL_SCANCODE_RIGHT:
  case SDL_SCANCODE_D:
    right.Emit();
    break;
  case SDL_SCANCODE_SPACE:
    trigger.Emit();
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