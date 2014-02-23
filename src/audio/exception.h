#ifndef EXCEPTION_H_
#define EXCEPTION_H_
#include <exception>
#include "boost/throw_exception.hpp"
#include "boost/exception/info.hpp"
namespace exception
{
class Exception : virtual public std::exception, virtual public boost::exception
{
};
}
#endif