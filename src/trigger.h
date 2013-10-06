#ifndef TRIGGER_H_
#define TRIGGER_H_

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
 class TriggerImpl* impl_;
};

};
#endif