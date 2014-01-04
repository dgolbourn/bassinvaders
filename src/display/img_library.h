#ifndef IMG_LIBRARY_H_
#define IMG_LIBRARY_H_

namespace img
{
class Library
{
public:
  Library(int flags);
  Library(Library const& other);
  Library(Library&& other);
  Library& operator=(Library other);
  ~Library(void);
};
}
#endif