#include "SDL_ttf.h"
#include "ttf_exception.h"

namespace ttf
{
int reference_count;

void init(void)
{
  if(!TTF_WasInit() && TTF_Init()==-1) 
  {
		throw Exception();
	}

	reference_count++;
}

void quit(void)
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

};