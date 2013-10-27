#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <memory>

namespace event
{

/**Callback is a function object called by Signal objects when their
Emit() method is called.  The Callback object should be created by an observer
who wishes to be informed when a Signal is emitted.*/
class Callback
{
public:
  /** operator() is called by the associated Signal object's Emit() method.*/
  virtual void operator() (class Signal&) = 0;
};

/**A Signal object is used to notify observers of an event.  When the event
occurs, the Signal's Emit() method should be called.  This will notify all
subscribed observers of the event via their subscribed Callback objects.*/
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

  /**Emit a signal.  Notify any observers via their subscribed Callback.*/
  void Emit(void);

  /**Subscribe a Callback to the Signal.
  @param[in] callback Reference to the Callback to subscribe.*/
  void Subscribe(Callback& callback);

  /**Unsubscribe a Callback from the Signal.
  @param[in] callback Reference to the Callback to unsubscribe.*/
  void Unsubscribe(Callback& callback);
private:
  std::shared_ptr<class SignalImpl> impl_;
};

}

#endif