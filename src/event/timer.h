#ifndef TIMER_H_
#define TIMER_H_
#include <memory>
#include "command.h"
#include "queue.h"
namespace event
{
class Timer
{
public:
  Timer(void) = default;
  Timer(int interval, event::Queue& queue);
  void Play(int loops = 0);
  void Pause(void);
  void Resume(void);
  void Add(Command const& command);
  void End(Command const& command);
private:
  std::shared_ptr<class TimerImpl> impl_;
};
}
#endif