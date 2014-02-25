#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_

#include <memory>
#include <string>
#include "json.h"
#include "weak_ptr.h"

namespace display
{
class BoundingBox
{
public:
  BoundingBox(void) = default;
  BoundingBox(float x, float y, float w, float h);
  BoundingBox(json::JSON const& json);
  bool operator&&(BoundingBox const& other) const;
  bool operator<(BoundingBox const& other) const;
  explicit operator bool(void) const;
  void x(float x);
  void y(float y);
  void w(float w);
  void h(float h);
  float x(void) const;
  float y(void) const;
  float w(void) const;
  float h(void) const;
  BoundingBox Copy(void) const;
  void Copy(BoundingBox const& other) const;
  typedef memory::WeakPtr<BoundingBox, class BoundingBoxImpl> WeakPtr;
private:
  std::shared_ptr<class BoundingBoxImpl> impl_;
  friend WeakPtr;
};
}
#endif