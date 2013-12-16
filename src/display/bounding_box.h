#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_

#include <memory>
#include <string>
#include "json.h"
#include "weak_ptr.h"

struct SDL_Rect;

namespace display
{
class BoundingBox
{
public:
  BoundingBox(void);
  BoundingBox(int x, int y, int w, int h);
  BoundingBox(std::string const& filename);
  BoundingBox(json::JSON const& json);
  bool operator&&(BoundingBox const& other) const;
  bool operator<(BoundingBox const& other) const;
  explicit operator bool(void) const;
  operator SDL_Rect*(void) const;
  int& x(void) const;
  int& y(void) const;
  int& w(void) const;
  int& h(void) const;
 
  BoundingBox(BoundingBox const& other);
  BoundingBox(BoundingBox&& other);
  ~BoundingBox(void);
  BoundingBox& operator=(BoundingBox other);

  typedef memory::WeakPtr<class BoundingBox, class BoundingBoxImpl> WeakPtr;
private:
  std::shared_ptr<class BoundingBoxImpl> impl_;
  friend WeakPtr;
};
}
#endif