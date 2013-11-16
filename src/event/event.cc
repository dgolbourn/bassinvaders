#include "event.h"
#include "SDL_events.h"
#include "sdl_manager.h"

namespace event
{
Signal quit;
Signal up;
Signal down;
Signal left;
Signal right;
Signal button1;

KeyMap key_map;

static void KeydownEvent(SDL_KeyboardEvent const& keyboard_event)
{
  auto iter = key_map.find(keyboard_event.keysym.scancode);
  if(iter != key_map.end())
  {
    iter->second.Notify();
  }
}

void Init(void)
{
  sdl::Init(SDL_INIT_EVENTS);
}

void Quit(void)
{
  sdl::Quit(SDL_INIT_EVENTS);
}

void DefaultKeys(void)
{
  key_map[SDL_SCANCODE_UP] = up;
  key_map[SDL_SCANCODE_W] = up;
  key_map[SDL_SCANCODE_DOWN] = down;
  key_map[SDL_SCANCODE_S] = down;
  key_map[SDL_SCANCODE_LEFT] = left;
  key_map[SDL_SCANCODE_A] = left;
  key_map[SDL_SCANCODE_RIGHT] = right;
  key_map[SDL_SCANCODE_D] = right;
  key_map[SDL_SCANCODE_SPACE] = button1;
}

void Event(void)
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