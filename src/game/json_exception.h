#ifndef JSON_EXCEPTION_H_
#define JSON_EXCEPTION_H_

#include <stdexcept>
#include "jansson.h"

namespace json
{
class Exception : public std::runtime_error 
{
public:
  Exception(json_error_t const& error);
};
}
#endif