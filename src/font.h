#ifndef FONT_H_
#define FONT_H_

#include <string>

class Font
{
  friend class WindowImpl;
public:
  Font(std::string filename, int point, int r, int g, int b);
  Font(void);
  Font(const Font& original);
  Font(Font&& original);
  ~Font(void);
  Font& operator=(Font original);

private:
  class FontImpl* impl_;
};

#endif