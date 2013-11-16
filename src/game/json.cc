#include "json.h"
#include "va_list.h"
#include "jansson.h"
#include "json_exception.h"

namespace json
{
JSON::JSON(void)
{
  json_ = json_null();
}

JSON::JSON(json_t* json)
{
  json_ = json_incref(json);
}

JSON::JSON(std::string const& filename)
{
  json_error_t error;
  json_ = json_load_file(filename.c_str(), 0, &error);
  if(!json_)
  {
    throw Exception(error);
  }
}

JSON::~JSON(void)
{
  json_object_clear(json_);
}

JSON::JSON(JSON const& other)
{
  json_ = json_incref(other.json_);
}

JSON::JSON(JSON&& other) : JSON()
{
  std::swap(json_, other.json_);
}

JSON& JSON::operator=(JSON other)
{
  std::swap(json_, other.json_);
  return *this;
}

void JSON::Unpack(std::string const& format, int dummy, ...) const
{
  json_error_t error;
  if(json_vunpack_ex(json_, &error, JSON_STRICT, format.c_str(), cstd::VAList<int>(dummy)) == -1)
  {
    throw json::Exception(error);
  }
}
}