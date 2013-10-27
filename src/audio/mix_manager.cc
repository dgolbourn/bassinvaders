#include <map>
#include <string>
#include "SDL_mixer.h"
#include "mix_manager.h"
#include "sdl_manager.h"
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

void Init(void)
{
  if(reference_count == 0)
  {
    sdl::Init(SDL_INIT_AUDIO);

    int const flags = 0;
    if((Mix_Init(flags) & flags) != flags) 
    {
      sdl::Quit(SDL_INIT_AUDIO);
      throw Exception();
    }

    int const samples = 1 << 10;
    if(Mix_OpenAudio(MIX_SAMPLE_RATE, MIX_FORMAT, MIX_CHANNEL_LAYOUT, samples) == -1)
    {
      sdl::Quit(SDL_INIT_AUDIO);
      MixQuit();
      throw Exception();
    }

    int const mixer_channels = 32;
    if(Mix_AllocateChannels(mixer_channels) != mixer_channels)
    {
      sdl::Quit(SDL_INIT_AUDIO);
      MixQuit();
      Mix_CloseAudio();
      throw Exception();
    }

    reference_count++;
  }
}

void Quit(void)
{
  if(reference_count > 0)
  {
    reference_count--;  
    if(reference_count == 0)
    {
      Mix_CloseAudio();
      MixQuit();
      sdl::Quit(SDL_INIT_AUDIO);
    }
  }
}

}