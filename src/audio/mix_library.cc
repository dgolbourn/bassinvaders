#include <map>
#include <string>
#include "SDL.h"
#include "SDL_mixer.h"
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
    static int const flags = 0;
    if((Mix_Init(flags) & flags) != flags) 
    {
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }

    static int const samples = static_cast<int>(1) << 10;
    if(Mix_OpenAudio(MIX_SAMPLE_RATE, MIX_FORMAT, MIX_CHANNEL_LAYOUT, samples) == -1)
    {
      MixQuit();
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }

    static int const mixer_channels = 128;
    if(Mix_AllocateChannels(mixer_channels) != mixer_channels)
    {
      Mix_CloseAudio();
      MixQuit();
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }
  }
  reference_count++;
}

Library::~Library(void)
{
  if(reference_count > 0)
  {
    --reference_count;  
    if(reference_count == 0)
    {
      Mix_CloseAudio();
      MixQuit();
    }
  }
}

Library::Library(Library const& other) : Library()
{
}

Library::Library(Library&& other) : Library()
{
}

Library& Library::operator=(Library other)
{
  return *this;
}
}