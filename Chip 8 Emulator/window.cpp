#include "window.h"
#include "chip.h"
#include <SDL.h>
#include <iostream>
#include <sstream>
#undef main

#define DEFAULTSPEED 13950.0

Window::Window(const char *title, int width, int height, int scale) :
	title(title), width(width), height(height), scale(scale)
{
	closed = !init();
}


Window::~Window()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(menuTexture);
	SDL_DestroyTexture(speedTexture);
	SDL_FreeSurface(textSurface);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_CloseAudioDevice(deviceId);
	SDL_FreeWAV(wavBuffer);
	SDL_Quit();
	delete speedString;
}


bool Window::init()
{
	emulationSpeed = DEFAULTSPEED; // Standard number of Ms to sleep after every cycle.

	//Initializes the use of SDL in general.
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cerr << "Unable to initialize SDL : " << SDL_GetError() << std::endl;
		return false;
	}

	//Create window and check if window was created.
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scale , height * scale , SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		std::cerr << "Failed to create window : " << SDL_GetError << std::endl;
		return false;
	}

	//Create renderer locked to afore mentioned window.
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL)
	{
		std::cerr << "Error creating renderer : " << SDL_GetError() << std::endl;
		return false;
	}

	if (SDL_RenderSetLogicalSize(renderer, width * scale, height * scale) < 0)
	{
		std::cerr << "Failed to set renderer logical size : " << SDL_GetError << std::endl;
		return false;
	}
	
	//Create texture
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (texture == NULL)
	{
		std::cerr << "Error creating texture : " << SDL_GetError() << std::endl;
		return false;
	}
	
	//Load sound WAV file
	if (SDL_LoadWAV("beep.wav", &wavSpec, &wavBuffer, &wavLength) == NULL)
	{
		std::cerr << "Error loading sound file : " << SDL_GetError() << std::endl;
		return false;
	}

	//Open Audio Device
	deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	if (deviceId == 0)
	{
		std::cerr << "Error identifying audio device : " << SDL_GetError() << std::endl;
		return false;
	}
	
	//Initialize SDL text
	if (TTF_Init() < 0)
	{
		std::cerr << "Error : " << TTF_GetError() << std::endl;
		return false;
	}

	font = TTF_OpenFont("Retro_Computer.ttf", 50);

	//Initialize Main Menu Text
	textSurface = TTF_RenderText_Blended_Wrapped(font,
	"+---------------+\n|  PAUSE MENU |\n+---------------+\n(ESC) : Resume\n(Q)      : Quit\n(R)      : Restart\n(N)      : New ROM\n(A)      : + Speed\n(D)      : - Speed\n",
												  white, 
												  width * scale);

	menuTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	//Set Menu to middle of screen
	menuRect.x = (width/4) * scale;		
	menuRect.y = 0;
	SDL_QueryTexture(menuTexture, NULL, NULL, &menuRect.w, &menuRect.h);

	return true;
}


void Window::draw()
{
	++drawCount;
	//Update Texture
	if (SDL_UpdateTexture(texture, NULL, pixel_buffer, width * sizeof(Uint32)) == 0)
	{
		//Render current pixel buffer.
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
	else
		closed = false;
}

void Window::pollEvents(chip &chip8)
{
	if (SDL_PollEvent(&event))
	{
		if (isPaused == false)
		{
			switch (event.type)
			{
				//Close window if user clicks the red 'x'.
			case SDL_QUIT:
				closed = true;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					//Close window if user presses "escape".
				case SDLK_ESCAPE:
					isPaused = !isPaused;
					displayPause();
					//closed = true;
					break;

					//Otherwise, check for a key press and update chip8 key array.
				default:
					for (int i = 0; i < 16; ++i)
						if (event.key.keysym.sym == keymap[i])
							chip8.key[i] = 1;
					break;
				}

				break;


			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					//Update chip8 key array when a key is released.
				default:
					for (int i = 0; i < 16; ++i)
						if (event.key.keysym.sym == keymap[i])
							chip8.key[i] = 0;
					break;
				}
				break;
			}
		}
		else if(isPaused ==true)
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_q:
						closed = true;
						break;

					case SDLK_r:
						needsReset = true;
						isPaused = !isPaused;
						chip8.cycle_count = 0;
						break;

					case SDLK_n:
						needsNewRom = true;
						needsReset = true;
						isPaused = !isPaused;
						chip8.cycle_count = 0;
						break;

					case SDLK_ESCAPE:
						isPaused = !isPaused;
						draw();
						break;

						//Allow user to change emulation speed with UP and DOWN arrow keys.
					case SDLK_LEFT:
						emulationSpeed += 1000;

						//Update Current Speed
						emulationRatio = DEFAULTSPEED / emulationSpeed;
						std::cout << "Emulation Speed Decreased : "  << emulationRatio << "x\n Emulation Speed is"<< emulationSpeed <<  std::endl;
						break;

					case SDLK_RIGHT:
						if (emulationSpeed > 1000)
							emulationSpeed -= 1000;
						else if (emulationSpeed > 100)
							emulationSpeed -= 100;
						else if (emulationSpeed > 10)
							emulationSpeed -= 10;
						else if (emulationSpeed > 1)
							emulationSpeed -= 1;

						//Update Current Speed
						emulationRatio = DEFAULTSPEED / emulationSpeed;
						std::cout << "Emulation Speed Increased : " << emulationRatio << "x\n Emulation Speed is"<< emulationSpeed <<std::endl;
						//speedString = stream.str().c_str();
						break;
				}
			}

		}
	}
}

void Window::pixelCopy(uint8_t *gfx)
{

	for (int i = 0; i < width * height; i++)
	{
		if (gfx[i] != 0)
			pixel_buffer[i] = primaryColor;
		else
			pixel_buffer[i] = 0;
	}
}

void Window::playSound()
{
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_PauseAudioDevice(deviceId, 0);
}

void Window::displayPause()
{
	++drawCount;
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer,menuTexture, NULL, &menuRect);
	SDL_RenderPresent(renderer);
}
