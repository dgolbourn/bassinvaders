#include "event.h"
#include "SDL.h"
#include "SDL_events.h"

namespace event
{
Signal quit;
Switch up;
Switch down;
Switch left;
Switch right;
Switch button1;
Switch pause;

KeyMap key_map;

static void KeydownEvent(SDL_KeyboardEvent const& keyboard_event, Uint32 const& type)
{
  if(keyboard_event.repeat == 0)
  {
    auto iter = key_map.find(keyboard_event.keysym.scancode);
    if(iter != key_map.end())
    {
      switch(type)
      {
      case SDL_KEYDOWN:
        iter->second.first();
        break;
      case SDL_KEYUP:
        iter->second.second();
        break;
      }
    }
  }
}

Event::Event(void) : sdl_(SDL_INIT_EVENTS)
{
}

Event::~Event(void)
{
}

void Default(void)
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
  key_map[SDL_SCANCODE_ESCAPE] = pause;
}

void Check(void)
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
    case SDL_QUIT:
      quit();
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      KeydownEvent(event.key , event.type);
      break;
    default:
      break;
    }
  }
}
}