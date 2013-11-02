#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_

#include <memory>

namespace display
{

class BoundingBox
{
  friend class TextureImpl;
public:
  BoundingBox(void);
  BoundingBox(int x, int y, int w, int h);
  BoundingBox(BoundingBox const& other);
  BoundingBox(BoundingBox&& other);
  ~BoundingBox(void);
  BoundingBox& operator=(BoundingBox other);

  /**Return true if both bounding boxes are identical.*/
  bool operator==(BoundingBox const& compare) const;

  /**Return the intersection of the two bounding boxes.*/
  BoundingBox operator|(BoundingBox const& other) const;

  /**Return the smallest bounding box that contains both boxes.*/
  BoundingBox operator&(BoundingBox const& other) const;

  /**Return true if the two bounding boxes overlap.*/
  bool operator&&(BoundingBox const& other) const;

  /**Return true if the bounding box has no extent.*/
  bool Empty(void) const;
private:
  std::shared_ptr<class BoundingBoxImpl> impl_;
};

}
#endif