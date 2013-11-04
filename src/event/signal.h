#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <memory>

namespace event
{

class NotifyCommandImpl
{
public:
  virtual void operator()(void) = 0;
};

typedef std::shared_ptr<NotifyCommandImpl> NotifyCommand;

class Signal
{
public:
  Signal(void);
  void Notify(void);
  void Subscribe(NotifyCommand const& notifiable);

  ~Signal(void);
  Signal(Signal const& other);
  Signal(Signal&& other);
  Signal& operator=(Signal other);
private:
  std::shared_ptr<class SignalImpl> impl_;
};

}

#endif