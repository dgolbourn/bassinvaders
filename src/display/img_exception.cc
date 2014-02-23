#include "img_exception.h"
#include "SDL_image.h"
namespace img
{
std::string Error(void)
{
  return IMG_GetError();
}
}