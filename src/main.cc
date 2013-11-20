#include <stdio.h>
#include <exception>
#include <iostream>
#include "window.h"
#include "texture.h"
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

class TestCallback : public event::CommandImpl
{
public:
  void operator()(void) final
  {
    printf("what!\n");
  }
};

bool quitflag = false;

class QuitCallback : public event::CommandImpl
{
public:
  void operator()(void) final
  {
    quitflag = true;
  }
};

int x;
int y;

class UpCallback : public event::CommandImpl
{
public:
  void operator()(void) final
  {
    printf("up!\n");
    y++;
  }
};

class DownCallback : public event::CommandImpl
{
public:
  void operator()(void) final
  {
    printf("down!\n");
    y--;
  } 
};

class LeftCallback : public event::CommandImpl
{
public:
  void operator()(void) final
  {
    printf("left!\n");
    x++;
  }
};

class RightCallback : public event::CommandImpl
{
public:
  void operator()(void) final
  {
    printf("right!\n");
    x--;
  }
};

class Play : public event::CommandImpl
{
public:
  audio::Sound sound_;

  void operator()(void) final
  {
    sound_.Play();
  }
};

class Show : public event::CommandImpl
{
public:
  game::Animation anim_;

  void operator()(void) final
  {
    display::BoundingBox box(50, 50, 250, 250);
    box.x() -= 10 * x;
    box.y() -= 10 * y;
    anim_.Render(box);
  }
};

class SpawnBaddie : public event::CommandImpl
{
public:
  void operator()(void) final
  {
  }
};

void HandleOutput(uint8_t* output, int out_samples)
{
  for(int i = 0; i < out_samples; i+=2)
  {
    std::cout << *((int16_t*)&output[i]) << " ";
  }
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

  w = display::Window("Bass Invaders");
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
  S3.Render();
  S2.Render(display::BoundingBox(), display::BoundingBox(20,20,20,20));
  S4.Render();
  S4.Render(15, 25);
  w.Show();

  event::Signal E;
  event::Command L(new TestCallback);
  E.Add(L);

  E.Notify();
  E.Notify();
  E.Notify();

  event::Command L0(new QuitCallback); event::quit.Add(L0);
  event::Command L2(new UpCallback); event::up.Add(L2);
  event::Command L3(new DownCallback); event::down.Add(L3);
  event::Command L4(new LeftCallback); event::left.Add(L4);
  event::Command L5(new RightCallback); event::right.Add(L5);
  audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
//  mixer.Music("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
  mixer.Resume();

  Play* play = new Play;
  play->sound_ = audio::Sound("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/high.wav");
  event::Command L6(play); event::button1.Add(L6);
  game::Animation anim("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/file.json", w);
  anim.Play(-1);

  Show* Sh = new Show;
  Sh->anim_ = anim;
  auto sr = event::Command(Sh);
  game::Scene Sc;
  Sc.Add(sr, 1);

  event::Timer timer(5000);
  timer.Pause();
  event::Command L7(new SpawnBaddie()); timer.Add(L7);
  timer.Resume();

  event::Signal pau;
  game::Collision col;
  game::Hero h("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json", w, Sc, col, pau);
  pau.Notify();
  h.x() = 100;
  h.y() = 100;

  while(!quitflag)
  {
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