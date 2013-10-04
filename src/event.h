#ifndef EVENT_H_
#define EVENT_H_

typedef void (*Callback) (class Event);

class Event
{
public:
  Event(void);
  ~Event(void);
  Event(const Event& original);
  Event(Event&& original);
  Event& operator=(Event original);

  void Emit(void);
  void Subscribe(Callback callback);
  void Unsubscribe(Callback callback);
private:
  class EventImpl* impl_;
};

#endif