#ifndef TTF_MANAGER_H_
#define TTF_MANAGER_H_

#include <SDL_ttf.h>

namespace ttf
{
/** Initialise the TTF api. If it is already active, increment its reference 
count.
@throw std::exception Text of exception contains information retrieved from 
TTF_GetError().*/
void init(void);

/** Decrement the reference count for the TTF api and then quit if the 
reference count is 0.*/
void quit(void);
};

#endif