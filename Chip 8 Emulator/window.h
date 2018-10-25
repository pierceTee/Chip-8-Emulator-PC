#pragma once
#include <iostream>
#include <string>
#include <SDL.h>
#include "chip.h"
#undef main


class Window
{
public:
	Window::Window(const char *title, int width, int height, int scale);
	~Window();

	void pollEvents(chip &chip8);
	void draw();
	void clear() const;
	void pixelCopy(uint8_t *gfx);

	bool isClosed() { return closed; }
	void playSound();
private:
	bool init();

private:
	const char *title;
	int width;
	int height;
	int scale;
	bool closed;
	//Initialize buffer to black for black background
	uint32_t pixel_buffer[64 * 32] = { 0 };
	int count = 0;
	Uint32 primaryColor = 0x0022E8A7;



public:
	 SDL_Window *window = nullptr;
	 SDL_Renderer *renderer = nullptr;
	 SDL_Texture *texture = nullptr;
	 SDL_AudioSpec wavSpec;
	 SDL_AudioDeviceID deviceId;
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