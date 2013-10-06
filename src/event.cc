#include "event.h"
#include "SDL_events.h"

Signal quit;

Signal up;
Signal down;
Signal left;
Signal right;

Signal trigger;

static void quit_event(void)
{
  quit.Emit();
}

static void keydown_event(SDL_KeyboardEvent& keyboard_event)
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

void events(void)
{
  SDL_Event events;
  while(SDL_PollEvent(&events))
  {
	  switch(events.type)
	  {
    case SDL_QUIT:
      quit_event();
      break;
    case SDL_KEYDOWN:
      keydown_event(events.key);
      break;
    default:
      break;
	  }
  }
}