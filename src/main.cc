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
#include "libavutil/mem.h"
#include "samples.h"
#include "buffer.h"

class TestCallback : public event::Notification
{
public:
  void operator()(void)
  {
    printf("what!\n");
  }
};

bool quitflag = false;

class QuitCallback : public event::Notification
{
public:
  void operator()(void)
  {
    quitflag = true;
    printf("who!\n");
  }
};

class UpCallback : public event::Notification
{
public:
  void operator()(void)
  {
    printf("up!\n");
  }
};

class DownCallback : public event::Notification
{
public:
  void operator()(void)
  {
    printf("down!\n");
  } 
};

class LeftCallback : public event::Notification
{
public:
  void operator()(void)
  {
    printf("left!\n");
  }
};

class RightCallback : public event::Notification
{
public:
  void operator()(void)
  {
    printf("right!\n");
  }
};

class Play : public event::Notification
{
public:
  audio::Sound sound_;

  void operator()(void)
  {
    sound_.Play();
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
  event::Observer L(new TestCallback);
  E.Subscribe(L);

  E.Notify();
  E.Notify();
  E.Notify();

  event::Observer L0(new QuitCallback); event::quit.Subscribe(L0);
  event::Observer L1(new QuitCallback); event::button1.Subscribe(L1);
  event::Observer L2(new UpCallback); event::up.Subscribe(L2);
  event::Observer L3(new DownCallback); event::down.Subscribe(L3);
  event::Observer L4(new LeftCallback); event::left.Subscribe(L4);
  event::Observer L5(new RightCallback); event::right.Subscribe(L5);
  audio::Mixer mixer;
  mixer.Music(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3"));
//  mixer.Music("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
  mixer.Resume();

  Play* play = new Play;
  play->sound_ = mixer.Load(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/high.wav"));;
  event::Observer L6(play); event::up.Subscribe(L6);
  event::Timer timer(1000);
  timer.Pause();
  event::Observer L7(new DownCallback); timer.Signal().Subscribe(L7);
  timer.Resume();

  game::Animation anim("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/file.json", w);

  while(!quitflag)
  {
    event::Event();
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