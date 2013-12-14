#ifndef FONT_H_
#define FONT_H_

#include <string>
#include <memory>

namespace display
{
class Font
{
  friend class WindowImpl;
  friend struct std::hash<display::Font>;
public:
  Font(std::string const& filename, int point, int r, int g, int b);
  Font(void);
  bool operator==(Font const& other) const;

  Font(Font const& other);
  Font(Font&& other);
  ~Font(void);
  Font& operator=(Font other);
private:
  std::shared_ptr<class FontImpl> impl_;
};
}

namespace std
{
template<> struct hash<display::Font> 
{
  size_t operator()(display::Font const& font) 
  { 
    return std::hash<std::shared_ptr<class display::FontImpl>>()(font.impl_);
  }
};
}
#endif