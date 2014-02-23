#ifndef JSON_H_
#define JSON_H_

#include <string>

struct json_t;

namespace json
{
class JSON
{
public:
  JSON(void);
  JSON(json_t* json);
  JSON(std::string const& filename);
  template<class... Args> void Unpack(std::string const& format, Args... args) const
  {
    Unpack_(format, 0, args...);
  }

  ~JSON(void);
  JSON(JSON const& other);
  JSON(JSON&& other);
  JSON& operator=(JSON other);
private:
  void Unpack_(std::string const& format, int dummy, ...) const;
  json_t* json_;
};
}
#endif