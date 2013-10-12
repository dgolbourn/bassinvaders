#ifndef SDL_MANAGER_H_
#define SDL_MANAGER_H_

#include <SDL.h>

namespace sdl
{
/**Initialise the requested SDL subsystems. If the subsystems are already 
active, increment the reference counts for those subsystems.
@param[in] flags Subsystem initialization flags as defined in SDL.h
@throw std::exception Text of exception contains information retrieved from 
SDL_GetError().*/
void Init(Uint32 flags);

/**Decrement the reference count for the indicated SDL subsystems.  Quit the
requested SDL subsystems if the reference count is 0.  If all subsystems are
inactive, then quit SDL.
@param[in] flags Subsystem initialization flags. See init().*/
void Quit(Uint32 flags);
}

#endif