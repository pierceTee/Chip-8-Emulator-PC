#include "window.h"
#include <SDL.h>
#include <iostream>
#include "chip.h"
#undef main


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
	SDL_CloseAudioDevice(deviceId);
	SDL_FreeWAV(wavBuffer);
	SDL_Quit();
}


bool Window::init()
{

	//Initializes the use of SDL in general, SDL_Init returns -1 on failure so we check that while calling it.
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cerr << "Unable to initialize SDL : " << SDL_GetError() << std::endl;
		return false;
	}

	//Create window and check if window was created.
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scale , height * scale , SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		std::cerr << "Failed to create window : " << SDL_GetError << std::endl;
		return false;
	}

	//Create renderer locked to afore mentioned window, check if renderer created. 
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
		//return false;
	}

	//Open Audio Device
	deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	if (deviceId == 0)
	{
		std::cerr << "Error identifying audio device : " << SDL_GetError() << std::endl;
		return false;
	}

	//play audio
	
	return true;
}

void Window::draw()
{

	//update texture
	if (SDL_UpdateTexture(texture, NULL, pixel_buffer, width * sizeof(Uint32)) == 0)
	{
			//clear screen and render
			//std::fill_n(pixel_buffer, 64 * 32, 0);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
	}
	else
	{
		closed = false;
	}
}

void Window::pollEvents(chip &chip8)
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{

		switch (event.type)
		{
		case SDL_QUIT:
			closed = true;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					closed = true;
					break;
				default:
					for (int i = 0; i < 16; ++i)
					{
						if (event.key.keysym.sym == keymap[i])
						{
							chip8.key[i] = 1;
							//std::cout << "You pressed : " << keymap[i]<< std::endl;
 						}
					}
					break;

			}
			break;
		
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
				default:
					for (int i = 0; i < 16; ++i)
					{
						if (event.key.keysym.sym == keymap[i])
						{
							chip8.key[i] = 0;
							//std::cout << "You released : " << keymap[i] << std::endl;
						}
					}
				break;
			}
			break;
		}

	}
}

void Window::clear() const
{
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}

void Window::pixelCopy(uint8_t *gfx)
{

	for (int i = 0; i < width * height; i++)
	{

	 if (gfx[i] != 0)
		{
			pixel_buffer[i] = primaryColor;
		}
		else
		{
			pixel_buffer[i] = 0;
		}
	}
}

void Window::playSound()
{
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_PauseAudioDevice(deviceId, 0);
}