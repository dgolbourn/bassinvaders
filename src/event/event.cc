#include "event.h"
#include "SDL_events.h"

namespace event
{
Signal quit;

Signal up;
Signal down;
Signal left;
Signal right;
Signal button1;

std::map<SDL_Scancode, Action> key_map;

static void KeydownEvent(SDL_KeyboardEvent& keyboard_event)
{
  auto iter = key_map.find(keyboard_event.keysym.scancode);
  if(iter != key_map.end())
  {
    switch(iter->second)
    {
    case UP:
      up.Notify();
      break;
    case DOWN:
      down.Notify();
      break;
    case LEFT:
      left.Notify();
      break;
    case RIGHT:
      right.Notify();
      break;
    case BUTTON1:
      button1.Notify();
      break;
    default:
      break;
    }
  }
}

void Init(void)
{
  key_map[SDL_SCANCODE_UP] = UP;
  key_map[SDL_SCANCODE_W] = UP;
  key_map[SDL_SCANCODE_DOWN] = DOWN;
  key_map[SDL_SCANCODE_S] = DOWN;
  key_map[SDL_SCANCODE_LEFT] = LEFT;
  key_map[SDL_SCANCODE_A] = LEFT;
  key_map[SDL_SCANCODE_RIGHT] = RIGHT;
  key_map[SDL_SCANCODE_D] = RIGHT;
  key_map[SDL_SCANCODE_SPACE] = BUTTON1;
}

void Events(void)
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
    case SDL_QUIT:
      quit.Notify();
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