#ifndef FONT_H_
#define FONT_H_

#include <string>
#include <memory>

namespace display
{

class Font
{
  friend class WindowImpl;
public:
  Font(std::string const& filename, int point, int r, int g, int b);
  Font(void);
  Font(Font const& other);
  Font(Font&& other);
  ~Font(void);
  Font& operator=(Font other);

private:
  std::shared_ptr<class FontImpl> impl_;
};

}
#endif