#include "json_exception.h"
#include <sstream>

namespace json
{

static std::string ErrorString(json_error_t const& error)
{
  std::stringstream message;
  message << "text: " << error.text << "\n" 
    << "source: " << error.source << "\n" 
    << "line: " << error.line << "\n" 
    << "column: " << error.column << "\n" 
    << "position: " << error.position;
  return message.str();
}

Exception::Exception(json_error_t const& error) : std::runtime_error(ErrorString(error).c_str())
{
}
}