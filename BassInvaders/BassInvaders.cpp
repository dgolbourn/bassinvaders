/*
 * BassInvaders.cpp
 *
 *  Created on: 19-Apr-2009
 *      Author: spijderman
 */

#include "BassInvaders.h"
#include "WindowManager.h"
#include "toolkit.h"
#include "spline.h"
#include <new>

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

		//((BassInvaders*)udata)->fft->ingest(stream);

		BeatDetector::process(((BassInvaders*)udata)->beat, sample->sample , len);
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
	delete fft;
	delete soundSource;
	delete beat;
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
	fft = new BandPassFilterFFT (soundSource->spec.freq, chunkSampleLength*4);
	dt = new BandPassFilterDT (chunkSampleLength*4);

	// set up the beat detector.
	int historyBuffer = (int) (1.0 / ((double)(chunkSampleLength)/(double)(soundSource->spec.freq)));
	beat = new BeatDetector(historyBuffer, SENSITIVITY, chunkSampleLength );
	beatIter = beat->iterator(COOLDOWN);

	// hook the game in to the music via the MusicPlayer function.
	Mix_HookMusic(BassInvaders::MusicPlayer, this);

	/* set up the HUD */
	SDL_Color c = {55, 255, 25};
	cout << "Loading HUD with font: " << (char*)((*level)["scorefont"]) << endl;
	pHUD = new hud((char*)((*level)["scorefont"]), 20, c, wm.getWindowSurface());
}

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

	/* this is just to test the monsters! This will eventually be managed by scenes! */
	static int stuff = 1;
	if (beatIter->isBeat())
	{
		Path path;
		path.x = &(defaultFunctors::monsterLinearX);	// Linear motion in x.
		if (stuff<8)
		{
			path.y = &(defaultFunctors::monsterConstantY); // constant motion in y.;
			randomHorde m(SCREEN_WIDTH, (rand()%SCREEN_HEIGHT - 50), path);
		}
		else
		{
			path.y = &(defaultFunctors::monsterSineY); // constant motion in y.;
			monsterLine m(SCREEN_WIDTH, (rand()%SCREEN_HEIGHT - 50), path, 5);
			stuff = 0;
		}
		stuff++;
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

