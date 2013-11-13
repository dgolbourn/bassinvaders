#ifndef ALARM_H_
#define ALARM_H_

#include <memory>
#include <string>
#include "mixer.h"
#include "json.h"

namespace game
{
class Alarm
{
public:
  Alarm(std::string const& filename, audio::Mixer& mixer);
  Alarm(json::JSON const& json, audio::Mixer& mixer);
  Alarm(void);
  void Pause(void);
  void Resume(void);

  ~Alarm(void);
  Alarm::Alarm(Alarm const& other);
  Alarm(Alarm&& other);
  Alarm& operator=(Alarm other);
private:
  std::shared_ptr<class AlarmImpl> impl_;
};
}
#endif