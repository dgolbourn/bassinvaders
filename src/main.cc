#include <stdio.h>
#include <exception>
#include <iostream>
#include "window.h"
#include "texture.h"
#include "cstd_exception.h"
#include "bounding_box.h"
#include "event.h"
#include "listener.h"

void test_callback(Event event)
{
  printf("what!\n");
}

int main(int argc, char *argv[]) 
{
  int ret = 0;
  Window* w = nullptr;
  Texture T;
  try
  {
    w = new Window();
    Texture S = w->Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
    T = w->Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
    T = w->Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  }
  catch(std::exception& e)
  {
    fprintf(stderr, "%s", e.what());
    ret = -1;
  }

  //std::cin.get();
  Texture S0 = w->Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  Texture S1 = w->Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  Texture S2 = w->Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");
  Texture S3 = w->Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/sprites/bulletred.bmp");

  {
  Font f("C:\\Users\\golbo_000\\Documents\\Visual Studio 2012\\Projects\\ReBassInvaders\\Debug\\lazy.ttf", 32, 255, 255, 255);
  }

  Font g("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/BassInvaders/BassInvaders/resources/fonts/Batang.ttf", 32, 255, 255, 255);

  Texture S4 = w->Text("the quick brown fox...", Font("C:\\Users\\golbo_000\\Documents\\Visual Studio 2012\\Projects\\ReBassInvaders\\Debug\\lazy.ttf", 72, 0, 0, 0));
  w->Clear();
  S3.Render();
  S2.Render(BoundingBox(), BoundingBox(20,20,20,20));
  S4.Render();
  w->Show();
  //std::cin.get();

  Event E;
  Listener L(test_callback, E);

  E.Emit();
  E.Emit();
  E.Emit();
  std::cin.get();
  delete w;
 
  try
  {
    cstd::Exception();
  }
  catch(...)
  {
  }

  return ret;
}