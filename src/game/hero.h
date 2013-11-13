#ifndef ENTITY_H_
#define ENTITY_H_

#include <memory>
#include <string>
#include "window.h"
#include "json.h"
#include "scene.h"
#include "signal.h"
#include "collision.h"
#include "mixer.h"

namespace game
{
class Hero
{
public:
  Hero(std::string const& filename, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer);
  Hero(json::JSON const& json, display::Window& window, Scene& scene, Collision const& collision, event::Signal& pause, audio::Mixer const& mixer);
  Hero(void);

  ~Hero(void);
  Hero::Hero(Hero const& other);
  Hero(Hero&& other);
  Hero& operator=(Hero other);
private:
  std::shared_ptr<class HeroImpl> impl_;
};
}
#endif