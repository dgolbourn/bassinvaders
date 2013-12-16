#include <stdio.h>
#include <exception>
#include <iostream>
#include "window.h"
#include "cstd_exception.h"
#include "bounding_box.h"
#include "signal.h"
#include "event.h"
#include "decoder.h"
#include "audio_format.h"
#include "music.h"
#include "sound.h"
#include "timer.h"
#include "animation.h"
#include "samples.h"
#include "buffer.h"
#include "scene.h"
#include "command.h"
#include "hero.h"
#include "collision.h"
#include "enemy.h"
#include "bind.h"

bool quitflag = false;

bool QuitCallback(void)
{
  quitflag = true;
  return true;
}

int main(int argc, char *argv[]) 
{
  int ret = 0;
  try
  {
  event::Event eL;
  event::DefaultKeys();
  display::Window w;
  display::Texture T;

  w = display::Window("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/window.json");
  display::Texture S = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  T = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  T = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");

  display::Texture S0 = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  display::Texture S1 = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  display::Texture S2 = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  display::Texture S3 = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");

  {
    display::Font f("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/lazy.ttf", 32, 255, 255, 255);
  }

  display::Font g("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/fonts/Batang.ttf", 32, 255, 255, 255);

  display::Texture S4 = w.Text("the quick brown fox...", display::Font("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/lazy.ttf", 32, 0, 0, 0));
  w.Clear();
  S3(display::BoundingBox(), display::BoundingBox(), 0.f, false, 0.);
  S2(display::BoundingBox(), display::BoundingBox(20,20,20,20), 0.f, false, 0.);
  S4(display::BoundingBox(), display::BoundingBox(), 0.f, false, 0.);
  w.Show();
  w.View(0,0,1.f);

  event::quit.Add(QuitCallback);
  audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
//  mixer.Music("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
  event::pause.first.Add(event::Bind(&audio::Music::Pause, mixer));
  event::pause.second.Add(event::Bind(&audio::Music::Resume, mixer));
  game::Scene Sc("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/scene.json", w);
  game::Collision col;
  game::Hero h("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json", w, Sc, col);
  h.Position(0, 0);
  w.View(0, 0, 5.f);
  float wx = 0;
  float wy = 0;
  float wa = 1.f;

  event::pause.second.Notify();
  while(!quitflag)
  {
    game::Position p = h.Position();
    wx = wx * (1.f - wa) + wa * float(p.first);
    wy = wy * (1.f - wa) + wa * float(p.second);
    w.View(int(wx), int(wy), 1.f);
    event::Check();
    w.Clear();
    Sc.Render();
    w.Show();
  }
  try
  {
    cstd::Exception();
  }
  catch(...)
  {
  }

  }
  catch(std::exception& e)
  {
    std::cout << e.what() << std::endl;
    std::cin.get();
    ret = -1;
  }
  return ret;
}