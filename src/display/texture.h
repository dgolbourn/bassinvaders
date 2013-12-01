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
  void Render(void) const;
  void Render(BoundingBox const& source, BoundingBox const& destination) const;
  float& Parallax(void);
  bool& Tile(void);

  Texture(Texture const& other);
  Texture(Texture&& other);
  ~Texture(void);
  Texture& operator=(Texture other);
private:
  std::shared_ptr<class TextureImpl> impl_;
};
}
#endif