#include "SDL_image.h"
#include "img_exception.h"

namespace img
{
int quit_flags;
int reference_count[8 * sizeof(int)];

void Init(int flags)
{
	if((flags & IMG_Init(flags)) != flags)
	{
		throw Exception();
	}

	int index = 0;
	while(flags)
	{
		if(flags & 0x01)
		{
			reference_count[index]++;
      quit_flags &= ~((int)0x01 << index);
		}
		flags >>= 1u;
		index++;
	}
}

void Quit(int flags)
{
	int index = 0;
	while(flags)
	{
		if(flags & 0x01)
		{
			if(reference_count[index] > 0)
			{
				reference_count[index]--;

				if(reference_count[index] == 0)
				{
					quit_flags |= ((int)0x01 << index);
				}
			}
		}
		flags >>= 1u;
		index++;
	}

	if(IMG_Init(0) == quit_flags)
	{
		IMG_Quit();
	}
}

}