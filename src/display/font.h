#ifndef FONT_H_
#define FONT_H_

#include <string>
#include <memory>

namespace display
{
class Font
{
public:
  Font(std::string const& filename, int point, int r, int g, int b, int a = 255);
  Font(void) noexcept;
private:
  std::shared_ptr<class FontImpl> impl_;
  friend class WindowImpl;
};
}
#endif