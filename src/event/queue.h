#ifndef QUEUE_H_
#define QUEUE_H_
#include "command.h"
#include <memory>
namespace event
{
class Queue
{
public:
  Queue(void);
  void Add(Command const& command);
  void operator()(void);
private:
  std::shared_ptr<class QueueImpl> impl_;
};
}
#endif