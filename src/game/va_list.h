#ifndef VA_LIST_H_
#define VA_LIST_H_
#include <cstdarg>

namespace cstd
{
template<class T> class VAList
{
public:
  VAList(T const& param_n)
  {
    va_start(ap_, param_n);
  }

  ~VAList(void)
  {
    va_end(ap_);
  }

  operator va_list(void) const
  {
    return ap_;
  }
private:
  va_list ap_;
};
}
#endif