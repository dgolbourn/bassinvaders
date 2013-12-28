#ifndef THREAD_H_
#define THREAD_H_
#include <mutex>
namespace thread
{
typedef std::lock_guard<std::mutex> Lock;
}
#endif