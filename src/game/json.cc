#include "json.h"
#include "json_exception.h"

namespace json
{
JSON::JSON(void)
{
  json_ = nullptr;
}

JSON::JSON(json_t* json)
{
  json_ = json;
}

JSON::~JSON(void)
{
  if(json_)
  {
    json_object_clear(json_);
  }
}

JSON::JSON(JSON const& other)
{
  json_ = json_incref(other.json_);
}

JSON::JSON(JSON&& other)
{
  json_ = other.json_;
  other.json_ = nullptr;
}

JSON& JSON::operator=(JSON other)
{
  std::swap(json_, other.json_);
  return *this;
}

json_t* JSON::Get(void) const
{
  return json_;
}

JSON Load(std::string const& filename)
{
  json_error_t error;
  json_t* json = json_load_file(filename.c_str(), 0, &error);
  if(!json) 
  {
    throw Exception(error);
  }
  return JSON(json);
}

}