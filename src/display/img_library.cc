#include "img_library.h"
#include "img_exception.h"
#include <climits>

namespace img
{
static int reference_count;

Library::Library(int flags)
{
  if((flags & IMG_Init(flags)) != flags)
  {
    throw Exception();
  }
  ++reference_count;
}

Library::~Library(void)
{
  --reference_count;
  if(reference_count == 0)
  {
    IMG_Quit();
  }
}
}