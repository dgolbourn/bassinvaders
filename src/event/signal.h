#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <memory>

namespace event
{

typedef void (*Callback) (class Signal);

class Signal
{
public:
  Signal(void);
  ~Signal(void);
  Signal(const Signal& original);
  Signal(Signal&& original);
  Signal& operator=(Signal original);

  void Emit(void);
  void Subscribe(Callback callback);
  void Unsubscribe(Callback callback);
private:
  std::shared_ptr<class SignalImpl> impl_;
};

}

#endif