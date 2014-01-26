#include <stdio.h>
#include <exception>
#include <iostream>
#include "window.h"
#include "bounding_box.h"
#include "signal.h"
#include "event.h"
#include "decoder.h"
#include "audio_format.h"
#include "music.h"
#include "sound.h"
#include "timer.h"
#include "animation.h"
#include "scene.h"
#include "command.h"
#include "hero.h"
#include "collision.h"
#include "enemy.h"
#include "bind.h"
#include "hud.h"
#include "rules_collision.h"
#include <atomic>

static std::atomic<bool> run(true);
static bool Quit(void)
{
  run = false;
  return false;
}

int main(int argc, char *argv[]) 
{
  int ret;
  try
  {
    event::Event eL;
    event::Default();
    event::quit.Add(Quit);
    display::Window w(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/window.json"));
    display::Texture S = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/petrified_rock_large__x1_iconic_png_1354840406.png");
    audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
    mixer.Volume(0.5);
    //audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
    event::pause.first.Add(event::Bind(&audio::Music::Pause, mixer));
    event::pause.second.Add(event::Bind(&audio::Music::Resume, mixer));
    game::Scene Sc(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/scene.json"), w);
    game::Collision col;
    game::RulesCollision rc(col);
    rc.Link(0, 1);
    game::Hero h(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json"), w, Sc, rc);
    h.End(Quit);
    game::HUD hud(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hud.json"), w, Sc);
    h.Life(event::Bind(&game::HUD::Life, hud));

    std::vector<display::BoundingBox> boxes(100);
    for(auto& box : boxes)
    {
      box = display::BoundingBox(-1500 + (rand() & 3000), -1500 + (rand() % 3000), 50, 50);
      event::Command c = std::bind(S, display::BoundingBox(), box, 1.f, false, 0.);
      Sc.Add(c, -1);

      game::RulesCollision::Send send = [=](){return std::pair<game::RulesCollision::Rules, bool>(game::RulesCollision::Rules(rand() %10, 0), true); };
      game::RulesCollision::Receive receive = [=](game::RulesCollision::Rules const& rules){(void)rules;  std::cout << "hit!" << std::endl; return true; };
      game::RulesCollision::Channel channel(send, receive);
      rc.Add(1, box, channel);
    }

    event::pause.second();
    int score = 0;
    while(run)
    {
      hud.Score(score++);
      game::Position p = h.Position();
      w.View(p.first, p.second, 1.f);
      event::Check();
      w.Clear();
      Sc.Render();
      w.Show();
      col.Check();
    }
    ret = EXIT_SUCCESS;
  }
  catch(std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    ret = EXIT_FAILURE;
  }
  catch(...)
  {
    ret = EXIT_FAILURE;
  }
  return ret;
}