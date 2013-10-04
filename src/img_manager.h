#ifndef IMG_MANAGER_H_
#define IMG_MANAGER_H_

#include <SDL_image.h>

namespace img
{
/** Load the requested image formats. Increment the reference counts for each
format to keep track of how many times it has been requested.
@param[in] flags Image formats mask as defined in SDL_image.h.
@throw std::exception Text of exception contains information retrieved from 
IMG_GetError().*/
void init(int flags);

/** Decrement the reference count for the indicated image formats.  Quit 
SDL_image once the reference count for all of the previously init-ed formats 
has reached 0.
@param[in] flags Image formats mask as defined in SDL_image.h.*/
void quit(int flags);
};

#endif