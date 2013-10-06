#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>
#include "texture.h"
#include "font.h"

namespace display
{

/**This class encapsulates a window and a compatible renderable context.  Use
this class to create a display window and load compatible textures from files.
*/
class Window
{
public:
/**@throw std::exception Text of exception contains information retrieved 
from SDL_GetError().*/
  Window(std::string name);
  Window(void);
  ~Window(void);
  Window(const Window& original);
  Window(Window&& original);
  Window& operator=(Window original);

/**Load the image indicated by filename.  This can be called multiple times 
on the same image file. The Texture object is effectively just a pointer to
an SDL texture which is managed such that it is only loaded once.
@param[in] filename The filename of the image to be loaded.
@return Texture object.
@throw std::exception Text of exception contains information retrieved from 
SDL_GetError() or IMG_GetError() as required.*/
  Texture Load(std::string filename);

  Texture Text(std::string text, Font font);

  void Clear(void);

  void Show(void);
private:
  class WindowImpl* impl_;
};

};
#endif