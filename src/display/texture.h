#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <memory>
#include "bounding_box.h"

namespace display
{
class Texture
{
  friend class WindowImpl;
public:
  Texture(void);
  void Render(BoundingBox const& source, BoundingBox const& destination, float parallax = 0.f, bool tile = false, double angle = 0.) const;
 
  Texture(Texture const& other);
  Texture(Texture&& other);
  ~Texture(void);
  Texture& operator=(Texture other);
private:
  std::weak_ptr<class TextureImpl> impl_;
};
}
#endif