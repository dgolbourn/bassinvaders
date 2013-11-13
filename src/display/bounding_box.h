#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_

#include <memory>
#include <string>
#include "json.h"

namespace display
{
class BoundingBox
{
  friend class TextureImpl;
public:
  BoundingBox(void);
  BoundingBox(int x, int y, int w, int h);
  BoundingBox(std::string const& filename);
  BoundingBox(json::JSON const& json);
  void Reset(void);
  bool operator==(BoundingBox const& other) const;
  bool operator&&(BoundingBox const& other) const;
  bool operator<(BoundingBox const& other) const;
  explicit operator bool(void) const;
  int& x(void) const;
  int& y(void) const;
  int& w(void) const;
  int& h(void) const;
 
  BoundingBox(BoundingBox const& other);
  BoundingBox(BoundingBox&& other);
  ~BoundingBox(void);
  BoundingBox& operator=(BoundingBox other);
private:
  std::shared_ptr<class BoundingBoxImpl> impl_;
};
}
#endif