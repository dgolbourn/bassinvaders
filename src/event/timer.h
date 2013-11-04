#ifndef TIMER_H_
#define TIMER_H_
#include <memory>
#include "signal.h"

namespace event
{
class Timer
{
public:
  Timer(void);
  Timer(int interval);
  void Pause(void);
  void Resume(void);
  Signal Signal(void);

  ~Timer(void);
  Timer(Timer const& other);
  Timer(Timer&& other);
  Timer& operator=(Timer other);
private:
  std::shared_ptr<class TimerImpl> impl_;
};
}
#endif