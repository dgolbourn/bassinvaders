#ifndef ENEMY_H_
#define ENEMY_H_

#include <memory>
#include <string>
#include "window.h"
#include "json.h"
#include "scene.h"
#include "signal.h"
#include "collision.h"

namespace game
{
class Enemy
{
public:
  Enemy(std::string const& filename, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause);
  Enemy(json::JSON const& json, display::Window& window, Scene& scene, Collision& collision, event::Signal& pause);
  Enemy(void);
  int& x(void);
  int& y(void);

  ~Enemy(void);
  Enemy::Enemy(Enemy const& other);
  Enemy(Enemy&& other);
  Enemy& operator=(Enemy other);
private:
  std::shared_ptr<class EnemyImpl> impl_;
};
}
#endif