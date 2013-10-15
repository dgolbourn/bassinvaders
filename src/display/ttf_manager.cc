#include "SDL_ttf.h"
#include "ttf_exception.h"

namespace ttf
{
static int reference_count;

void Init(void)
{
  if(!TTF_WasInit() && TTF_Init()==-1) 
  {
    throw Exception();
  }

  reference_count++;
}

void Quit(void)
{
  if(reference_count > 0)
  {
    reference_count--;

    if(reference_count == 0)
    {
      TTF_Quit();
    }
  }
}

}