#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "chip.h"
#undef main


class Window
{
public:
	Window::Window(const char *title, int width, int height, int scale);
	~Window();

	void pollEvents(chip &chip8);
	void draw();
	void pixelCopy(uint8_t *gfx);

	bool isClosed() { return closed; }
	void playSound();
	int emulationSpeed;				//How long to sleep the thread after each emulation cycle in ms.
	void bindKeys();
	bool isPaused = false;
	bool needsReset = false;
	bool needsNewRom = false;
	void displayPause();
private:
	bool init();

private:
	const char *title;
	int width;
	int height;
	int scale;
	bool closed;
	const char *speedString = nullptr;		//String used to print out current emulation speed.
	int drawCount = 0;
	double emulationRatio = 1;
	std::stringstream stream;
	uint32_t pixel_buffer[64 * 32] = { 0 };	//Initialize buffer to black for black background
	Uint32 primaryColor = 0x0022E8A7;



public:
	 SDL_Window *window = nullptr;

	 SDL_Renderer *renderer = nullptr;

	 SDL_Texture *texture = nullptr;
	 SDL_Texture *menuTexture = nullptr;
	 SDL_Texture *speedTexture = nullptr;

	 SDL_Surface *textSurface = nullptr;

	 TTF_Font *font = nullptr;
	 SDL_Color white = { 255, 255, 255, 255 };

	 SDL_Rect menuRect;
	 SDL_Rect speedRect;

	 SDL_AudioSpec wavSpec;
	 SDL_AudioDeviceID deviceId;
	 SDL_Event event;

	 Uint32 wavLength;
	 Uint8 *wavBuffer;
	 uint8_t keymap[16] = {
		 SDLK_x,
		 SDLK_1,
		 SDLK_2,
		 SDLK_3,
		 SDLK_q,
		 SDLK_w,
		 SDLK_e,
		 SDLK_a,
		 SDLK_s,
		 SDLK_d,
		 SDLK_z,
		 SDLK_c,
		 SDLK_4,
		 SDLK_r,
		 SDLK_f,
		 SDLK_v,
	 };
};
#pragma once