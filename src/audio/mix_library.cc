#include <map>
#include <string>
#include "SDL.h"
#include "mix_library.h"
#include "mix_library.h"
#include "sdl_library.h"
#include "mix_exception.h"
#include "audio_format.h"

namespace mix
{
static int reference_count;

static void MixQuit(void)
{
  while(Mix_Init(0))
  {
    Mix_Quit();
  }
}

Library::Library(void) : sdl_(SDL_INIT_AUDIO)
{
  if(reference_count == 0)
  {
    int const flags = 0;
    if((Mix_Init(flags) & flags) != flags) 
    {
      throw Exception();
    }

    int const samples = static_cast<int>(1) << 10;
    if(Mix_OpenAudio(MIX_SAMPLE_RATE, MIX_FORMAT, MIX_CHANNEL_LAYOUT, samples) == -1)
    {
      MixQuit();
      throw Exception();
    }

    int const mixer_channels = 32;
    if(Mix_AllocateChannels(mixer_channels) != mixer_channels)
    {
      Mix_CloseAudio();
      MixQuit();
      throw Exception();
    }
  }
  reference_count++;
}

Library::~Library(void)
{
  if(reference_count > 0)
  {
    reference_count--;  
    if(reference_count == 0)
    {
      Mix_CloseAudio();
      MixQuit();
    }
  }
}
}