#ifndef CHANNEL_H_
#define CHANNEL_H_
#include "command.h"
namespace event
{
template<class Message> class Channel
{
public:
  typedef std::function<std::pair<Message, bool>(void)> Send;
  typedef std::function<bool(Message const&)> Receive;
  typedef std::pair<Send, Receive> Pair;
};

template<class Channel> Command Bind(Channel a, Channel b)
{
  return[=]()
  {
    bool valid = false;
    auto b_message = b.first();
    if(b_message.second)
    {
      if(a.second(b_message.first))
      {
        auto a_message = a.first();
        if(a_message.second)
        {
          if(b.second(a_message.first))
          {
            valid = true;
          }
        }
      }
    }
    return valid;
  };
}
}
#endif