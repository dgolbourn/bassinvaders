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

  ~Signal(void);
  Signal(Signal const& other);
  Signal(Signal&& other);
  Signal& operator=(Signal other);
private:
  std::shared_ptr<class SignalImpl> impl_;
};
}
#endif