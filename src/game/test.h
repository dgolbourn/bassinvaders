#ifndef TEST_H_
#define TEST_H_
#include <memory>
class Test
{
public:
  Test();
  ~Test();
  Test(Test &&);
  Test& operator=(Test &&);
private:
  std::unique_ptr<class Impl> impl_;
};
#endif