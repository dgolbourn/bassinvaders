#ifndef TIMER_H_
#define TIMER_H_
#include <memory>
#include <vector>
#include "signal.h"

namespace event
{
class Timer
{
public:
  Timer(void);
  Timer(int interval, bool repeats);
  ~Timer(void);
  Timer::Timer(const Timer& original);
  Timer(Timer&& original);
  Timer& operator=(Timer original);

  void Pause(void);
  void Resume(void);
  Signal Signal(void);
private:
  std::shared_ptr<class TimerImpl> impl_;
};
}
#endif