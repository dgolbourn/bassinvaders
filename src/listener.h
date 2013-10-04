#ifndef LISTENER_H_
#define LISTENER_H_

#include "event.h"

class Listener
{
public:
  Listener(void);
  Listener(Callback callback, Event event);
  ~Listener(void);
  Listener(const Listener& original);
  Listener(Listener&& original);
  Listener& operator=(Listener original);
private:
 class ListenerImpl* impl_;
};

#endif