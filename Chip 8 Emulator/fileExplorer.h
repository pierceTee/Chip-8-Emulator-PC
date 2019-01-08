#ifndef FILE_EXPLORER_H
#define FILE_EXPLORER_H

#include <tinyfiledialogs.h>
#include <tinyfiledialogs.c>
//#include <nfd.h>
#include <stdio.h>
#include <stdlib.h>

// "C:\\development\\chip8_roms\\"
const char* fileExplore()
{
	const char *result = tinyfd_openFileDialog("Select a chip-8 ROM file", NULL, 0, NULL, NULL, 0);

	if (result) 
	{
		puts("Success!");
		puts(result);
		return result;
	}
	else 
	{
		puts("User pressed cancel.");
		return NULL;

	}
	
	
}

#endif //FILE_ECPLORER_H