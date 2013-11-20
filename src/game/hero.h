#ifndef HERO_H_
#define HERO_H_

#include <memory>
#include <string>
#include "window.h"
#include "json.h"
#include "scene.h"
#include "signal.h"
#include "collision.h"

namespace game
{
class Hero
{
public:
  Hero(std::string const& filename, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause);
  Hero(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause);
  Hero(void);
  int& x(void);
  int& y(void);

  ~Hero(void);
  Hero::Hero(Hero const& other);
  Hero(Hero&& other);
  Hero& operator=(Hero other);
private:
  std::shared_ptr<class HeroImpl> impl_;
};
}
#endif