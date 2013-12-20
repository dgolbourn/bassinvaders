#ifndef ENEMY_H_
#define ENEMY_H_

#include <memory>
#include <string>
#include "window.h"
#include "json.h"
#include "scene.h"
#include "signal.h"
#include "collision.h"
#include "position.h"
#include "role.h"

namespace game
{
class Enemy
{
public:
  Enemy(std::string const& filename, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause, Role& role);
  Enemy(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause, Role& role);
  Enemy(void);
  void Position(int x, int y);
  game::Position Position(void);
  void End(event::Command const& command);

  ~Enemy(void);
  Enemy(Enemy const& other);
  Enemy(Enemy&& other);
  Enemy& operator=(Enemy other);
private:
  std::shared_ptr<class EnemyImpl> impl_;
};
}
#endif