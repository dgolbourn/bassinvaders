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

void TestCallback(event::Signal signal)
{
  printf("what!\n");
}

bool quitflag = false;

void QuitCallback(event::Signal signal)
{
  quitflag = true;
  printf("who!\n");
}

void UpCallback(event::Signal signal)
{
  printf("up!\n");
}

void DownCallback(event::Signal signal)
{
  printf("down!\n");
}

void LeftCallback(event::Signal signal)
{
  printf("left!\n");
}

void RightCallback(event::Signal signal)
{
  printf("right!\n");
}

int main(int argc, char *argv[]) 
{
  int ret = 0;
  display::Window w;
  display::Texture T;
  try
  {
    w = display::Window("Bass Invaders");
    display::Texture S = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
    T = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
    T = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  }
  catch(std::exception& e)
  {
    fprintf(stderr, "%s", e.what());
    ret = -1;
  }

  display::Texture S0 = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  display::Texture S1 = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  display::Texture S2 = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  display::Texture S3 = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");

  {
    display::Font f("C:\\Users\\golbo_000\\Documents\\Visual Studio 2012\\Projects\\ReBassInvaders\\Debug\\lazy.ttf", 32, 255, 255, 255);
  }

  display::Font g("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/fonts/Batang.ttf", 32, 255, 255, 255);

  display::Texture S4 = w.Text("the quick brown fox...", display::Font("C:\\Users\\golbo_000\\Documents\\Visual Studio 2012\\Projects\\ReBassInvaders\\Debug\\lazy.ttf", 32, 0, 0, 0));
  w.Clear();
  S3.Render();
  S2.Render(display::BoundingBox(), display::BoundingBox(20,20,20,20));
  S4.Render();
  S4.Render(15, 25);
  w.Show();

  event::Signal E;
  event::Trigger L(TestCallback, E);

  E.Emit();
  E.Emit();
  E.Emit();
  
  event::Trigger L0(QuitCallback, event::quit);
  event::Trigger L1(QuitCallback, event::trigger);
  event::Trigger L2(UpCallback, event::up);
  event::Trigger L3(DownCallback, event::down);
  event::Trigger L4(LeftCallback, event::left);
  event::Trigger L5(RightCallback, event::right);

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

  return ret;
}