#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <memory>

namespace event
{

class Notification
{
public:
  virtual void operator()(void) = 0;
};

typedef std::shared_ptr<Notification> Observer;

/**A Signal object is used to notify observers of an event.  When the event
occurs, the Signal's Notify() method should be called by the subject.  This
will call the Notification function objects of each subscribed Observer.*/
class Signal
{
public:
  /**Constructor.  Construct a new Signal.*/
  Signal(void);

  /**Destructor.  Decrement the reference count to this Signal.  If this is
  the last reference, destroy the Signal.*/
  ~Signal(void);

  /**Copy constructor.  Make this Signal be a reference to other.
  @param[in] other Signal to reference.*/
  Signal(Signal const& other);

  /**Move constructor.  Make this Signal be a reference to other.
  @param[in] other Signal to reference.*/
  Signal(Signal&& other);

  /**Move assignment operator.  Make this Signal be a reference to other.
  @param[in] other Signal to reference.
  @return Reference to this.*/
  Signal& operator=(Signal other);

  /**Notify Observers of an event via their Notify methods.*/
  void Notify(void);

  /**Subscribe an Observer to the Signal.
  @param[in] observer Reference to the Observer to subscribe.*/
  void Subscribe(Observer const& observer);
private:
  std::shared_ptr<class SignalImpl> impl_;
};

}

#endif