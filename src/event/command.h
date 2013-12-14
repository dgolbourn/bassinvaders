#ifndef COMMAND_H_
#define COMMAND_H_
#include <functional>
namespace event
{
typedef std::function<bool(void)> Command;
}
#endif