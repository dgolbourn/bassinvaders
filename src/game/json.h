#ifndef JSON_H_
#define JSON_H_

#include "jansson.h"
#include <string>

namespace json
{
class JSON
{
public:
  JSON(void);
  JSON(json_t* json);
  ~JSON(void);
  JSON(JSON const& other);
  JSON(JSON&& other);
  JSON& operator=(JSON other);

  json_t* Get(void);
private:
  json_t* json_;
};

JSON Load(std::string& filename);

}
#endif