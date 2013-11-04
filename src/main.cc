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
#include "mixer.h"
#include "sound.h"
#include "timer.h"
#include "animation.h"
#include "samples.h"
#include "buffer.h"
#include "scene.h"

class TestCallback : public event::NotifyCommandImpl
{
public:
  void operator()(void)
  {
    printf("what!\n");
  }
};

bool quitflag = false;

class QuitCallback : public event::NotifyCommandImpl
{
public:
  void operator()(void)
  {
    printf("who!\n");
  }
};

int x;
int y;

class UpCallback : public event::NotifyCommandImpl
{
public:
  void operator()(void)
  {
    printf("up!\n");
    y++;
  }
};

class DownCallback : public event::NotifyCommandImpl
{
public:
  void operator()(void)
  {
    printf("down!\n");
    y--;
  } 
};

class LeftCallback : public event::NotifyCommandImpl
{
public:
  void operator()(void)
  {
    printf("left!\n");
    x++;
  }
};

class RightCallback : public event::NotifyCommandImpl
{
public:
  void operator()(void)
  {
    printf("right!\n");
    x--;
  }
};

class Play : public event::NotifyCommandImpl
{
public:
  audio::Sound sound_;

  void operator()(void)
  {
    sound_.Play();
  }
};

class Show : public game::RenderCommandImpl
{
public:
  game::Animation anim_;

  void operator()(void)
  {
    anim_.Render(display::BoundingBox(50-10*x,50-10*y,250,250));
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
  event::Init();
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
  event::NotifyCommand L(new TestCallback);
  E.Subscribe(L);

  E.Notify();
  E.Notify();
  E.Notify();

  event::NotifyCommand L0(new QuitCallback); event::quit.Subscribe(L0);
  event::NotifyCommand L1(new QuitCallback); event::button1.Subscribe(L1);
  event::NotifyCommand L2(new UpCallback); event::up.Subscribe(L2);
  event::NotifyCommand L3(new DownCallback); event::down.Subscribe(L3);
  event::NotifyCommand L4(new LeftCallback); event::left.Subscribe(L4);
  event::NotifyCommand L5(new RightCallback); event::right.Subscribe(L5);
  audio::Mixer mixer;
  mixer.Music("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
//  mixer.Music("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
  mixer.Resume();

  Play* play = new Play;
  play->sound_ = mixer.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/high.wav");
  event::NotifyCommand L6(play); event::button1.Subscribe(L6);
  event::Timer timer(1000);
  timer.Pause();
  event::NotifyCommand L7(new DownCallback); timer.Signal().Subscribe(L7);
  timer.Resume();

  game::Animation anim("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/file.json", w);
  anim.Resume();

  Show* Sh = new Show;
  Sh->anim_ = anim;
  auto sr = game::RenderCommand(Sh);

  game::Scene Sc;
  Sc.Subscribe(sr, 0);

  while(!quitflag)
  {
    event::Event();
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