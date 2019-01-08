#include <iostream>
#include <SDL.h>
#include <chrono>
#include <thread>
#include "window.h"
#include "chip.h"
#include "fileExplorer.h"
//#undef main

/*
	TODO:
		- Refactor code, add comments. 

		- add pause menu with options to : restart game, start new game, exit.

		- port to android.

		- port to switch.
*/

int main(int argc, char **argv)
{

	//Create Chip and Window
	chip chip8;
	Window window("Chip-8", 64 , 32, 20);
	const char * currentROM = fileExplore();
	//Load ROM
	if (!chip8.load(currentROM))
	{
		std::cerr << "Unable to load specified ROM file, please try again." << std::endl;
		return -1;
	}

	//Emulation Loop
	while (!window.isClosed())
	{
		window.pollEvents(chip8);
		//Run 9 cycles per frame, then render.
		for (int cycles = 0; cycles < 9; cycles++)
		{
			chip8.emulateCycle();
		}
			if (!window.isPaused)
			{
				//chip8.emulateCycle();
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

				if (window.needsReset == true)
				{
					if (window.needsNewRom == true)
					{
						currentROM = fileExplore();
						if (!chip8.load(currentROM))
						{
							std::cerr << "Unable to load specified ROM file, please try again." << std::endl;
							return -1;
						}
						window.needsNewRom = false;
					}
					else
					{
						if (!chip8.load(currentROM))
						{
							std::cerr << "Unable to load specified ROM file, please try again." << std::endl;
							return -1;
						}
					}

					window.needsReset = false;
				}
			}
		//Sleep the thread to slow down emulation speed.
		std::this_thread::sleep_for(std::chrono::microseconds(window.emulationSpeed));

	}
	return 0;
}