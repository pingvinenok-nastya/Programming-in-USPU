/*
 * main.cc
 *
 *  Created on: 27 ���. 2020 �.
 *      Author: 1234
 */


#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#ifdef __WIN32__
#include <winsock.h>
#else
#include <netinet/in.h>
#endif
#include <vector>
#include "AudioDriver.h"
#include "SoundSorce.h"
#include "FMWaveGenerator.h"

int main(int, char**)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	FMInstrument inst
			{ 5, true, {
			  { 1., 0.1, 0.9, 0.000001, 1.0, 0.},
			  { 1., 0.1, 0.9, 0.000001, 1.0, 0. },
			  { 1., 0.1, 0.9, 0.000001, 1.0, 0.},
			  { 1., 0.1, 0.9, 0.000001, 1.0, 0.} } };

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


static const double s_freq_table[]={0.};
std::vector<std::vector<char>> track;

	std::fstream file;

	file.open("MIDI_sample.mid", std::ios::in | std::ios::binary);

	int chunk_type;
	int MThd_length;
	short MThd_format;
	short MThd_tracks;
	short MThd_division;


	// ������ ���������
	file.read(reinterpret_cast<char*>(&chunk_type), sizeof(chunk_type));
	if (chunk_type == (
			'M'*0x00000001 +
			'T'*0x00000100 +
			'h'*0x00010000 +
			'd'*0x01000000))
		std::cout << "�, ��� ���������!" << std::endl;

	file.read(reinterpret_cast<char*>(&MThd_length), sizeof(MThd_length));
	MThd_length = ntohl(MThd_length);
	std::cout << "MThd length: " << MThd_length << std::endl;

	file.read(reinterpret_cast<char*>(&MThd_format), sizeof(MThd_format));
	MThd_format = ntohs(MThd_format);
	std::cout << "MThd format: " << MThd_format << std::endl;

	file.read(reinterpret_cast<char*>(&MThd_tracks), sizeof(MThd_tracks));
	MThd_tracks = ntohs(MThd_tracks);
	std::cout << "MThd tracks: " << MThd_tracks << std::endl;

	file.read(reinterpret_cast<char*>(&MThd_division), sizeof(MThd_division));
	MThd_division = ntohs(MThd_division);
	std::cout << "MThd division: " << MThd_division << std::endl;
	if (MThd_division & 0x8000) {
		std::cout << "������� ��������� - SMPTE" << std::endl;
	} else {
		std::cout << "������� ��������� - TPQN" << std::endl;
	}

	track.resize(MThd_tracks);


	for (int i = 0; i < MThd_tracks; i++) {
		file.read(reinterpret_cast<char*>(&chunk_type), sizeof(chunk_type));
		if (chunk_type == (
				'M'*0x00000001 +
				'T'*0x00000100 +
				'r'*0x00010000 +
				'k'*0x01000000)) {
			std::cout << "�, � ��� ������� " << i << std::endl;
		} else {
			std::cout << "��, ��� �� ������� :(" << std::endl;
			break;
		}
		int trk_length;
		file.read(reinterpret_cast<char*>(&trk_length), sizeof(int));
		trk_length = ntohl(trk_length);
		std::cout << "  �����: " << trk_length << std::endl;
		track[i].resize(trk_length);
		file.read(&track[i][0], trk_length);
	}

	size_t trk = 0;
	std::cout << " " << std::endl;

	size_t p = 0;
	while (p < track[trk].size()) {
		int delta = 0;
		int delta_v;

		do {
		  delta_v = track[trk][p++];
		  delta *= 128;
		  delta += delta_v & 0x7f;
		} while (delta_v & 0x80);

		std::cout << std::setw(10) << delta << ":---> ";

		unsigned int event_type = 0x000000ff & track[trk][p++];

		std::cout << "���� ������� " << event_type ;

		switch(event_type) {
		case 0x80 ... 0x8f:
			wg->key_off();
			break;
		case 0x90 ... 0x9f:
			eg->set_level(track[trk][p++]);
			wg->set_freq(s_freq_table[int(track[trk][p++])]);
			wg->key_on();
			break;
		case 0xa0 ... 0xaf:
			std::cout << " ��� �����������3" << std::endl;
			break;
		case 0xb0 ... 0xbf:
			std::cout << " ��� �����������4" << std::endl;
			break;
		case 0xc0 ... 0xcf:
			std::cout << " ��� �����������5" << std::endl;
			break;
		case 0xd0 ... 0xdf:
			std::cout << " ��� �����������6" << std::endl;
			break;
		case 0xe0 ... 0xef:
			std::cout << " ��� �����������7" << std::endl;
			break;
		case 0xf0:
			std::cout << " ��� �����������8" << std::endl;
			break;
		case 0xf1:
			std::cout << " ��� �����������9" << std::endl;
			break;
		case 0xf2:
			std::cout << " ��� �����������10" << std::endl;
			break;
		case 0xf3:
			std::cout << " ��� �����������12" << std::endl;
			break;
		case 0xf4:
			std::cout << " ��� �����������13" << std::endl;
			break;
		case 0xf5:
			std::cout << " ��� �����������14" << std::endl;
			break;
		case 0xf6:
			std::cout << " ��� �����������15" << std::endl;
			break;
		case 0xf7:
			std::cout << " ��� �����������16" << std::endl;
			break;
		case 0xf8:
			std::cout << " ��� �����������17" << std::endl;
			break;
		case 0xf9:
			std::cout << " ��� �����������18" << std::endl;
			break;
		case 0xfa:
			std::cout << " ��� �����������19" << std::endl;
			break;
		case 0xfb:
			std::cout << " ��� �����������20" << std::endl;
			break;
		case 0xfc:
			std::cout << " ��� �����������21" << std::endl;
			break;
		case 0xfd:
			std::cout << " ��� �����������22" << std::endl;
			break;
		case 0xfe:
			std::cout << " ��� �����������23" << std::endl;
			break;
		case 0xff:
			switch(int(track[trk][p++])) {
			case 0x01:

				break;
			case 0x02:

				break;
			case 0x03:
				std::cout << " ��� �����������@@@@@@ " << int(track[trk][p++]) <<std::endl;
				break;
			case 0x04:

				break;
			}
			std::cout << " ��� �����������24" << std::endl;
			break;

		default:
			std::cout << "�����, � ������ ������� �� ����!" << std::endl;
			return 1;
		}
	}





	file.close();

	return 0;
}


