#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <memory>
#include "command.h"

namespace event
{
class Signal
{
public:
  Signal(void);
  void operator()(void);
  void Add(Command const& command);
private:
  std::shared_ptr<class SignalImpl> impl_;
};
}
#endif