#ifndef HERO_H_
#define HERO_H_

#include <memory>
#include <string>
#include "window.h"
#include "json.h"
#include "scene.h"
#include "signal.h"
#include "rules_collision.h"
#include "position.h"
#include "weak_ptr.h"

namespace game
{
class Hero
{
public:
  Hero(json::JSON const& json, display::Window& window, Scene& scene, RulesCollision& collision);
  Hero(void) = default;
  void Position(game::Position const& position);
  game::Position Position(void);
  typedef std::function<bool(int)> Command;
  void Life(Command const& command);
  void End(event::Command const& command);
  typedef memory::WeakPtr<Hero, class HeroImpl> WeakPtr;
private:
  std::shared_ptr<class HeroImpl> impl_;
  friend WeakPtr;
};
}
#endif