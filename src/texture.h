#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "bounding_box.h"

/**The Texture object is an encapsulation of an SDL_Texture. The SDL_Texture 
is only stored once, and the number of Texture objects referencing it are
remembered.  Construction, copy, move, assignment and destruction operators are
overloaded so that Texture objects can and should be passed around by value and
everything in terms of memory management of the SDL_Texture should just work.*/
class Texture
{
  friend class WindowImpl;
public:
  Texture(void);
  Texture(const Texture& original);
  Texture(Texture&& original);
  ~Texture(void);
  Texture& operator=(Texture original);

  void Render(void);
  void Render(BoundingBox& source, BoundingBox& destination);
private:
  class TextureImpl* impl_;
};

#endif