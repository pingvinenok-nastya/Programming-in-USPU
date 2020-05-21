/*
 * main.cc
 *
 *  Created on: 27 мар. 2020 г.
 *      Author: 1234
 */


#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#include "AudioDriver.h"
#include "SoundSorce.h"
#include "FMWaveGenerator.h"

int main(int, char**)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	FMInstrument inst
			{ 5, true, {
			  { 1., 1., 1.,1.,0.1,1.},
			  { 1., 1., 1.,1.,0.1,1. },
			  { 1., 1., 1.,1.,0.1,1.},
			  { 1., 1., 1.,1.,0.1,1.} } };

	AudioDriver drv;
	drv.unpause();

	auto src = std::make_shared<SoundSource>();
	auto wg = std::make_shared<FMWaveGenerator>();
	auto eg = std::make_shared<ConstantEnvelopeGenerator>(1.);


	wg->set_freq(440.);

	wg->setup(inst);
	src->set_envelope_generator(eg);
	src->set_wave_generator(wg);

	drv.set_sound_source(src);
	drv.set_master_volume(4000);
	wg->key_on();
	wg->key_off();

	auto win = std::shared_ptr<SDL_Window>(
			SDL_CreateWindow("", 50, 50, 800, 600, SDL_WINDOW_SHOWN),
			SDL_DestroyWindow);
	SDL_Event ev;
	for (;;) {
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) goto the_exit;
		}

		SDL_Delay(10);
	}

the_exit:


	return 0;
}


