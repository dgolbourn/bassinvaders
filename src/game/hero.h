#ifndef HERO_H_
#define HERO_H_

#include <memory>
#include <string>
#include "window.h"
#include "json.h"
#include "scene.h"
#include "signal.h"
#include "collision.h"
#include "position.h"
#include "weak_ptr.h"

namespace game
{
class Hero
{
public:
  Hero(std::string const& filename, display::Window& window, Scene& scene, Collision& collision);
  Hero(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision);
  Hero(void);
  void Position(game::Position const& position);
  game::Position Position(void);
  void Life(std::function<bool(int)> const& command);
  void End(event::Command const& command);

  ~Hero(void);
  Hero(Hero const& other);
  Hero(Hero&& other);
  Hero& operator=(Hero other);
  typedef memory::WeakPtr<Hero, class HeroImpl> WeakPtr;
private:
  std::shared_ptr<class HeroImpl> impl_;
  friend WeakPtr;
};
}
#endif