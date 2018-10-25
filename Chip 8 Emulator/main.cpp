//#include "stdafx.h"
#include <iostream>
//#include <chrono>
//#include <thread>
#include "chip.h"
#include <SDL.h>
#include <chrono>
#include <thread>
#include "window.h"
#undef main


int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Please chose a file to load.\nUsage: chip8 <ROM file>" << std::endl;
		return -1;
	}
	std::cout << argv[0] << "\n" << argv[1] << std::endl;
	 

	//chip chip8 = chip();
	chip chip8;
	Window window("Chip-8", 64 , 32, 20);
	
	//load ROM
	if (!chip8.load(argv[1]))
	{
		std::cerr << "Unable to load specified ROM file, please try again." << std::endl;
		return -1;
	}

	while (!window.isClosed())
	{
		//SDL_Delay(01);
		chip8.emulateCycle();
		window.pollEvents(chip8);
		if (chip8.needsRedraw == true)
		{
			window.pixelCopy(chip8.gfx);
			window.draw();
			chip8.needsRedraw = false;
		}
		if (chip8.needSound)
		{
			window.playSound();
			chip8.needSound = false;
		}
		window.pollEvents(chip8);
		
		std::this_thread::sleep_for(std::chrono::microseconds(180));

	}
	return 0;
}