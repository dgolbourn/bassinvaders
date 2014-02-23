#ifndef JSON_EXCEPTION_H_
#define JSON_EXCEPTION_H_
#include "exception.h"
#include <string>
namespace json
{
class Exception : virtual public exception::Exception
{
public:
  typedef boost::error_info<class Text, std::string> Text;
  typedef boost::error_info<class Source, std::string> Source;
  typedef boost::error_info<class Line, int> Line;
  typedef boost::error_info<class Column, int> Column;
  typedef boost::error_info<class Position, int> Position;
};
}
#endif