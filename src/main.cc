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
#include <chrono>
#include "dynamics.h"
#include "dynamics_collision.h"
#include <thread>
#include "boost/exception/diagnostic_information.hpp"
#include "ffmpeg_exception.h"
#include "img_exception.h"
static bool run = true;
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
    display::Texture S = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/black.png");
    //audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
    audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
    mixer.Volume(0.5);
    mixer.Play();
    mixer.Pause();
    event::pause.first.Add(event::Bind(&audio::Music::Pause, mixer));
    event::pause.second.Add(event::Bind(&audio::Music::Resume, mixer));
    game::Scene Sc(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/scene.json"), w);
    game::Collision col;
    game::RulesCollision rc(col);
    rc.Link(0, 1);
    game::DynamicsCollision dc(col);
    dc.Link(0, 1);
    dc.Link(1, 2);
    dc.Link(2, 2);
    dc.Link(0, 2);
    event::Queue queue;
    game::Hero h(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json"), w, Sc, rc, dc, queue);
    h.End(Quit);
    game::HUD hud(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hud.json"), w, Sc);
    h.Life(event::Bind(&game::HUD::Life, hud));
    
    std::vector<display::BoundingBox> boxes(100);
    std::vector<game::Dynamics> dynamics(100);
    int idx = 0;
    for(auto& box : boxes)
    {
      box = display::BoundingBox(200+rand() % 3000, 200+rand() % 1500, 50, 50);
      event::Command c = std::bind(S, display::BoundingBox(), box, 1.f, false, 0.);
      Sc.Add(c, -1);

      game::RulesCollision::Send send = [=](){return std::pair<game::RulesCollision::Rules, bool>(game::RulesCollision::Rules(rand() %10, 0), true); };
      game::RulesCollision::Receive receive = [=](game::RulesCollision::Rules const& rules){(void)rules;  std::cout << "hit!" << std::endl; return true; };
      game::RulesCollision::Channel channel(send, receive);
      //rc.Add(1, box, channel);

      dynamics[idx] = game::Dynamics(box.x() + 0.5f * box.w(), box.y() + 0.5f* box.h(), 0, 0, box.w(), box.h(), (rand() % 1000)/2000.f);
      dc.Add(1, dynamics[idx], box);
      dynamics[idx].k(0.99);
      if(idx % 10 == 0)
      {
        dc.Add(2, dynamics[idx], box);
        dynamics[idx].b(1000);
        dynamics[idx].m(1);
        dynamics[idx].d(0.9f);
      }
      ++idx;
    }

    event::pause.second();

    std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point tick = last;
    int frame_rate = 60;
    std::chrono::milliseconds frame_period_ms(1000/frame_rate);
    float frame_period_s = 1.f/frame_rate;
    while(run)
    {
      std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
      float dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - last).count();
      last = now;
      int current_frame_rate = int(std::round(1./dt));
      hud.Score(current_frame_rate);
      game::Position p = h.Position();
      int idx = 0;
      for(auto& box : boxes)
      {
        box.x(dynamics[idx].x() - .5f * box.w());
        box.y(dynamics[idx].y() - .5f * box.h());
        ++idx;
      }
      w.View(p.first, p.second, 1.f);
      w.Clear();
      Sc.Render();
      w.Show();
      event::Check();
      queue();
      h.Step(frame_period_s);
      for(auto& d : dynamics)
      {
        d.Step(frame_period_s);
      }
      col.Check();
      tick += frame_period_ms;
      std::this_thread::sleep_until(tick);
    }
    ret = EXIT_SUCCESS;
  }
  catch(...)
  {
    std::cerr << "Unhandled exception:" << std::endl << boost::current_exception_diagnostic_information();
    ret = EXIT_FAILURE;
  }
  return ret;
}