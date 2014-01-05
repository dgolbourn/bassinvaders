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
  BoundingBox(json::JSON const& json);
  bool operator&&(BoundingBox const& other) const;
  bool operator<(BoundingBox const& other) const;
  explicit operator bool(void) const;
  operator SDL_Rect(void) const;
  void x(int x);
  void y(int y);
  void w(int w);
  void h(int h);
  int x(void) const;
  int y(void) const;
  int w(void) const;
  int h(void) const;
  BoundingBox Copy(void) const;
  void Copy(BoundingBox const& other) const;
  typedef memory::WeakPtr<BoundingBox, class BoundingBoxImpl> WeakPtr;
  friend WeakPtr;
private:
  std::shared_ptr<class BoundingBoxImpl> impl_;
};
}
#endif