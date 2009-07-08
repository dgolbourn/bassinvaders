/*
 * BassInvaders.cpp
 *
 *  Created on: 19-Apr-2009
 *      Author: spijderman
 */

#include "BassInvaders.h"
#include "WindowManager.h"
#include "toolkit.h"
#include "path.h"
#include "spline.h"
#include <boost/numeric/ublas/matrix.hpp>
#include "BeatManager.h"
#include <iostream>
/*
 * This is called by SDL Music for chunkSampleSize x 4 bytes each time SDL needs it
 */
void BassInvaders::MusicPlayer(void *udata, Uint8 *stream, int len)
{
	SoundSourceIterator* iter = ((BassInvaders*)udata)->soundIter;
	if(iter->hasNext())
	{
		SoundSample * sample = iter->next();
		memcpy(stream, sample->sample, sample->len);

		BeatManager::process(((BassInvaders*)udata)->pBM, sample->sample);
	}
}

BassInvaders * BassInvaders::theGame = 0;

BassInvaders::BassInvaders()
{
	pHero = NULL;
	pBG = NULL;
	gameState = Initialising;
	nextState = Loading;
	running = true;
	BassInvaders::theGame = this;
}

BassInvaders::~BassInvaders() {
	delete pHero;
	//delete pBG; // TODO Background needs reworking to use the resourceBundle anyway.
	delete dt;
	delete soundSource;
	//delete beat;
	delete pBM;
}

void BassInvaders::goGameGo()
{
	while (running)
	{
		update();
		switch (gameState)
		{
			case Loading:
			{
				doLoadingState();
			}
			break;

			case Playing:
			{
				doPlayingState();
			}
			break;

			case Paused:
			{
				doPausedState();
			}break;

			default:
			{
			}
			break;
		}

		// draw whatever has been done
		if(running)
			wm.Flip();
	}
}

void BassInvaders::injectState(GameStates_t newState)
{
	nextState = newState;
}

void BassInvaders::update()
{
	if (gameState != nextState)
	{
		gameState = nextState;
		switch(gameState)
		{
			case Loading:
			{
				loadLevel();
			}break;

			case Playing:
			{
				pBG->play();
			}break;

			case Paused:
			{
				pauseGame();
			}break;

			default:
			{
			}break;
		}
		DebugPrint(("Moving to state: %u\n", gameState));
	}
}

/**************************
 * Loading logic of game loop
 *************************/
void BassInvaders::doLoadingState()
{
	injectState(Playing);
}

void BassInvaders::loadLevel()
{
	pRM = new EntityManager(wm.getWindowSurface());

	/* Load the level */
	ResourceBundle *level = ResourceBundle::getResource("resources/levels/level-test.info");

	/* set up background */

	pBG = new Background(1, 10, SCREEN_HEIGHT, SCREEN_WIDTH);
	LayerInfo_t bgLayer;
	memset(&bgLayer, 0, sizeof(LayerInfo_t));
	pBG->createLayerFromFile(&bgLayer, "resources/background/b0.info");
	pBG->addLayer(&bgLayer);
	memset(&bgLayer, 0, sizeof(LayerInfo_t));
	pBG->createLayerFromFile(&bgLayer, "resources/background/b1.info");
	pBG->addLayer(&bgLayer);
	memset(&bgLayer, 0, sizeof(LayerInfo_t));
	pBG->createLayerFromFile(&bgLayer, "resources/background/b2.info");
	pBG->addLayer(&bgLayer);

	// create the hero and stuff him into the Entity manager
	pHero = new Hero(ResourceBundle::getResource("resources/hero/heroclass.info"), pRM);
	pRM->setHero(pHero);

	/*
	 * Set up the music playback, filters and beat detection environment
	 */
	// where the music comes from.
	soundSource = (SoundSource*)((*level)["music"]);

	// this is how many 2 x 2byte samples are in a chunk
	int chunkSampleLength = soundSource->spec.samples;

	// What the music is played by.
	// OpenAudio should be initialised with chunk_size = samples
	// and soundIter should be set up to take the appropriate number of bytes (i.e. samples x 4).
	Mix_OpenAudio( soundSource->spec.freq, MIX_DEFAULT_FORMAT, soundSource->spec.channels, chunkSampleLength);
	soundIter = soundSource->iter(chunkSampleLength*4);

	// fft and dt filters if you want them.
	dt = new BandPassFilterDT (chunkSampleLength*4);

	// set up the beat detector.
	pBM = new BeatManager(chunkSampleLength, soundSource->spec.freq);

	beat0 = pBM->detector(1.7, 1,80);
	beat = pBM->detector(1.7, 80,160);
	beat1 = pBM->detector(1.7, 160,320);
	beat2 = pBM->detector(1.7, 320,640);
	beat3 = pBM->detector(1.7, 640,1280);
	beat4 = pBM->detector(1.7, 1280,20000);

	// hook the game in to the music via the MusicPlayer function.
	Mix_HookMusic(BassInvaders::MusicPlayer, this);

	/* set up the HUD */
	SDL_Color c = {55, 255, 25};
	cout << "Loading HUD with font: " << (char*)((*level)["scorefont"]) << endl;
	pHUD = new hud((char*)((*level)["scorefont"]), 20, c, wm.getWindowSurface());
}

struct X{
	beat_t beat;
	int32_t pos;
	bool operator<(const X x) const{
		return  this->beat < x.beat;
	}
};

/**************************
 * Playing logic of game loop
 *************************/
void BassInvaders::doPlayingState()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			running = false;
			return;
		}

		if (event.type == SDL_KEYUP)
		{
			if ((event.key.keysym.sym == SDLK_p) && (event.key.state == SDL_RELEASED))
			{
				injectState(Paused);
			}
		}

		static int isRegistered = 0;
		if (event.type == SDL_KEYUP)
		{
			if ((event.key.keysym.sym == SDLK_a) && (event.key.state == SDL_RELEASED))
			{
				pBG->accelerate(10, 1);
				BandPassFilterDT::alpha = 0.3;
				if (!isRegistered)
				{
					Mix_RegisterEffect(MIX_CHANNEL_POST, BandPassFilterDT::lowPassFilterEffect, NULL, dt);
					isRegistered = 1;
				}
			}

			if ((event.key.keysym.sym == SDLK_d) && (event.key.state == SDL_RELEASED))
			{
				pBG->accelerate(1, 1);
				BandPassFilterDT::alpha = 1.;
			}
		}
	}

	/* firstly, draw the background */
	pBG->redraw(wm.getWindowSurface());

	/* move the hero about and let him shoot things*/
	pHero->setActions(im.getCurrentActions());

	static uint32_t now, now2;
	static uint32_t delta, delta2;
	static uint32_t lastTickCount = 0, lastTickCount2=0;

	now = now2 = SDL_GetTicks();
	delta = now - lastTickCount;
	delta2 = now2 - lastTickCount2;

	std::vector<X> findMax1(3);
	//std::vector<X> findMax2(3);
	findMax1[0].beat = beat0->isBeat(); findMax1[0].pos = 1;
	//findMax2[2].beat = beat4->isBeat(); findMax2[2].pos = 6;
	//findMax1[1].beat = beat->isBeat(); findMax1[1].pos = 2;
	//findMax1[2].beat = beat1->isBeat();findMax1[2].pos = 3;
	//findMax2[0].beat = beat2->isBeat();findMax2[0].pos = 4;
	//findMax2[1].beat = beat3->isBeat();findMax2[1].pos = 5;

	X Y = (*max_element(findMax1.begin(),findMax1.end()));
	//X Z = (*max_element(findMax2.begin(),findMax2.end()));

	/* this is just to test the monsters! This will eventually be managed by scenes! */
	if ((Y.beat) && (delta > COOLDOWN))
	{
		lastTickCount = now;

		/*
		 * monsters are given paths to move along, this might turn out to be a little
		 * cumbersome for easy paths like just going straight forward, but for complicated
		 * beautiful splines I think it will turn out to be a good way :-)
		 */
		Path path;

		/*
		 * set up an example transformation, rotation around the middle of the screen
		 */
		//apply_transform(path.defaultStack, affine_translate(SCREEN_WIDTH/2, SCREEN_HEIGHT/2) );
		//apply_transform(path.defaultStack, affine_rotate(rand()) );
		//apply_transform(path.defaultStack, affine_translate(-SCREEN_WIDTH/2, -SCREEN_HEIGHT/2) );

		/*
		 * translate enemy into its start position
		 */
		apply_transform(path.defaultStack, affine_translate(SCREEN_WIDTH, Y.pos*(SCREEN_HEIGHT/7) ));

		/*
		 * set up the path taken by the enemy
		 */
		path.x = &(defaultFunctors::monsterLinearX);	// Linear motion in x.
		if (rand()%50 != 1)
		{
			path.y = &(defaultFunctors::monsterConstantY); // constant motion in y.;
			randomHorde m(path);
		}
		else
		{
			path.y = &(defaultFunctors::monsterSineY); // sine motion in y.;
			path.ds = 0.9;
			monsterLine m(path,5);
		}
	}

	/* do collision detection */
	pRM->doCollisions();

	pRM->update();

	pRM->move();

	/* draw all the active Entities */
	pRM->render();

	/* remove the dead/off screen ones */
	pRM->removeInactiveEntities();

	/* draw the hud/overlay */
	pHUD->displayText(10,10,"Health: %i",pHero->getHealth());
	pHUD->displayText(300,10,"Score: %i0",pHero->score);

	pHUD->draw();
}

/**************************
 * Paused logic of game loop
 *************************/

void BassInvaders::pauseGame()
{
	pBG->pause();
}

void BassInvaders::doPausedState()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			running = false;
		}

		if (event.type == SDL_KEYUP)
		{
			if ((event.key.keysym.sym == SDLK_p) &&
					(event.key.state == SDL_RELEASED))
			{
				injectState(Playing);
			}
		}
	}

	pBG->redraw(wm.getWindowSurface()); //it isn't really redrawing, it's updating the bg's tick counts
}

