#include "ttf_library.h"
#include "SDL_ttf.h"
#include "ttf_exception.h"

namespace ttf
{
static int reference_count;

Library::Library(void)
{
  if(!TTF_WasInit() && TTF_Init()==-1) 
  {
    throw Exception();
  }
  ++reference_count;
}

Library::~Library(void)
{
  if(reference_count > 0)
  {
    --reference_count;
    if(reference_count == 0)
    {
      TTF_Quit();
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