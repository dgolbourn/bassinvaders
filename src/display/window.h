#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>
#include <memory>
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
  Window(std::string const& name);
  Window(void);
  ~Window(void);
  Window(Window const& other);
  Window(Window&& other);
  Window& operator=(Window other);

/**Load the image indicated by filename.  This can be called multiple times 
on the same image file. The Texture object is effectively just a pointer to
an SDL texture which is managed such that it is only loaded once.
@param[in] filename The filename of the image to be loaded.
@return Texture object.
@throw std::exception Text of exception contains information retrieved from 
SDL_GetError() or IMG_GetError() as required.*/
  Texture Load(std::string const& filename);

  Texture Text(std::string const& text, Font const& font);

  void Free(std::string const& filename);

  void Clear(void) const;

  void Show(void) const;
private:
  std::shared_ptr<class WindowImpl> impl_;
};

}
#endif