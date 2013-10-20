#ifndef TRIGGER_H_
#define TRIGGER_H_

#include <memory>
#include "signal.h"

namespace event
{

/**Trigger objects are used by observers to manage the subscription and 
unsubscription of their Callbacks from Signals.  By using a Trigger to 
associate a Callback with a Signal, this ensures that the Callback is 
unsubscribed correctly when the Trigger goes out of scope.*/
class Trigger
{
public:
  /**Constructor.  Create an empty Trigger.*/
  Trigger(void);

  /**Constructor.  Create a Trigger associated with signal and callback.
  @param[in] callback Callback to subscribe to signal.
  @param[in] signal Signal to subscribe Callback to.*/
  Trigger(Callback& callback, Signal signal);

  /**Destructor.  Decrement the reference count to this Trigger.  If this is
  the last reference, destroy the Trigger.*/
  ~Trigger(void);

  /**Copy constructor.  Make this Trigger be a reference to original.
  @param[in] original Trigger to reference.*/
  Trigger(const Trigger& original);

  /**Move constructor.  Make this Trigger be a reference to original.
  @param[in] original Trigger to reference.*/
  Trigger(Trigger&& original);

  /**Move assignment operator.  Make this Trigger be a reference to original.
  @param[in] original Trigger to reference.
  @return Reference to this.*/
  Trigger& operator=(Trigger original);
private:
  std::shared_ptr<class TriggerImpl> impl_;
};

}
#endif