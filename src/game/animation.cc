#include "animation.h"
//#include "texture.h"
//#include "trigger.h"
//#include "timer.h"
#include "jansson.h"
#include <iostream>
#include "json_exception.h"
#include "json.h"

namespace game
{
class AnimationImpl
{
public:
  //display::Texture texture_;
  //event::Trigger trigger_;
  //event::Timer timer_;
  void JanssonTest(void);
};

void AnimationImpl::JanssonTest(void)
{
  json::JSON json = json::Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/file.json");

  const char* str;
  int i, h, w;
  json_t* list;
  json_error_t error;
  if(json_unpack_ex(json.Get(), &error, JSON_STRICT, "{sssisisiso}", 
    "sprite sheet", &str, 
    "interval", &i,
    "height", &h,
    "width", &w,
    "frames", &list) == -1)
  { 
    throw json::Exception(error);
  }
  else
  {
    std::cout << str << std::endl;
    std::cout << i << std::endl;
    std::cout << h << std::endl;
    std::cout << w << std::endl;

    size_t index;
    json_t* value;
    json_array_foreach(list, index, value)
    {
      int a[2];
      if(json_unpack_ex(value, &error, JSON_STRICT, "[ii]", &a[0], &a[1]) == -1)
      {
        throw json::Exception(error);
      }
      else
      {
        std::cout << a[0] << std::endl;
        std::cout << a[1] << std::endl;
      }
    }
  }
}

Animation::Animation(void)
{
  impl_ = std::shared_ptr<AnimationImpl>(new AnimationImpl);
}

void Animation::Test(void)
{
  impl_->JanssonTest();
}

}