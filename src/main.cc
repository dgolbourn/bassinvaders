#include <stdio.h>
#include <exception>
#include <iostream>
#include "window.h"
#include "texture.h"
#include "cstd_exception.h"
#include "bounding_box.h"
#include "signal.h"
#include "trigger.h"
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

class TestCallback : public event::Callback
{
public:
  void operator()(event::Signal& signal)
  {
    printf("what!\n");
  }
};

bool quitflag = false;

class QuitCallback : public event::Callback
{
public:
  void operator()(event::Signal& signal)
  {
    quitflag = true;
    printf("who!\n");
  }
};

class UpCallback : public event::Callback
{
public:
  void operator()(event::Signal& signal)
  {
    printf("up!\n");
  }
};

class DownCallback : public event::Callback
{
public:
  void operator()(event::Signal& signal)
  {
    printf("down!\n");
  } 
};

class LeftCallback : public event::Callback
{
public:
  void operator()(event::Signal& signal)
  {
    printf("left!\n");
  }
};

class RightCallback : public event::Callback
{
public:
  void operator()(event::Signal& signal)
  {
    printf("right!\n");
  }
};

class Play : public event::Callback
{
public:
  audio::Sound sound_;

  void operator()(event::Signal& signal)
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
  
  display::Window w;
  display::Texture T;

  w = display::Window(std::string("Bass Invaders"));
  display::Texture S = w.Load(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp"));
  T = w.Load(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp"));
  T = w.Load(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp"));

  display::Texture S0 = w.Load(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp"));
  display::Texture S1 = w.Load(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp"));
  display::Texture S2 = w.Load(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp"));
  display::Texture S3 = w.Load(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp"));

  {
    display::Font f(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/lazy.ttf"), 32, 255, 255, 255);
  }

  display::Font g(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/fonts/Batang.ttf"), 32, 255, 255, 255);

  display::Texture S4 = w.Text(std::string("the quick brown fox..."), display::Font(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/lazy.ttf"), 32, 0, 0, 0));
  w.Clear();
  S3.Render();
  S2.Render(display::BoundingBox(), display::BoundingBox(20,20,20,20));
  S4.Render();
  S4.Render(15, 25);
  w.Show();

  event::Signal E;
  event::Trigger L(TestCallback(), E);

  E.Emit();
  E.Emit();
  E.Emit();
  
  event::Trigger L0(QuitCallback(), event::quit);
  event::Trigger L1(QuitCallback(), event::trigger);
  event::Trigger L2(UpCallback(), event::up);
  event::Trigger L3(DownCallback(), event::down);
  event::Trigger L4(LeftCallback(), event::left);
  event::Trigger L5(RightCallback(), event::right);
  audio::Mixer mixer;
  Play play;

  play.sound_ = mixer.Load(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/high.wav"));
  mixer.Music(std::string("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3"));
//  mixer.Music("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
  mixer.Resume();

  event::Trigger L6(play, event::up);
  event::Timer timer(1000, true);
  timer.Pause();
  event::Trigger L7(DownCallback(), timer.Signal());
  timer.Resume();

  game::Animation anim;

  anim.Test();

  while(!quitflag)
  {
    event::Events();
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