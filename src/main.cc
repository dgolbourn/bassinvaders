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

void test_callback(event::Signal signal)
{
  printf("what!\n");
}

bool quitflag = false;

void quit_callback(event::Signal signal)
{
  quitflag = true;
  printf("who!\n");
}

void up_callback(event::Signal signal)
{
  printf("up!\n");
}

void down_callback(event::Signal signal)
{
  printf("down!\n");
}

void left_callback(event::Signal signal)
{
  printf("left!\n");
}

void right_callback(event::Signal signal)
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

  display::Texture S4 = w.Text("the quick brown fox...", display::Font("C:\\Users\\golbo_000\\Documents\\Visual Studio 2012\\Projects\\ReBassInvaders\\Debug\\lazy.ttf", 72, 0, 0, 0));
  w.Clear();
  S3.Render();
  S2.Render(display::BoundingBox(), display::BoundingBox(20,20,20,20));
  S4.Render();
  S4.Render(15, 25);
  w.Show();

  event::Signal E;
  event::Trigger L(test_callback, E);

  E.Emit();
  E.Emit();
  E.Emit();
  
  event::Trigger L0(quit_callback, event::quit);
  event::Trigger L1(quit_callback, event::trigger);
  event::Trigger L2(up_callback, event::up);
  event::Trigger L3(down_callback, event::down);
  event::Trigger L4(left_callback, event::left);
  event::Trigger L5(right_callback, event::right);

  while(!quitflag)
  {
    event::events();
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