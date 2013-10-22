#ifndef JSON_EXCEPTION_H_
#define JSON_EXCEPTION_H_

#include <stdexcept>
#include "jansson.h"

namespace json
{
/**json error exception.  You can throw this if a json function returns an 
error.*/
class Exception : public std::runtime_error 
{
public:
  Exception(json_error_t const& error);
};
}

#endif