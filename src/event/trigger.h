#ifndef TRIGGER_H_
#define TRIGGER_H_

#include <memory>
#include "signal.h"

namespace event
{

class Trigger
{
public:
  Trigger(void);
  Trigger(Callback callback, Signal signal);
  ~Trigger(void);
  Trigger(const Trigger& original);
  Trigger(Trigger&& original);
  Trigger& operator=(Trigger original);
private:
  std::shared_ptr<class TriggerImpl> impl_;
};

}
#endif