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
  void Unpack(std::string const& format, int dummy, ...) const;

  ~JSON(void);
  JSON(JSON const& other);
  JSON(JSON&& other);
  JSON& operator=(JSON other);
private:
  json_t* json_;
};
}
#endif