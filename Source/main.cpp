#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Engine/Platform.h>
#include "NemoGame.h"

int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PSTR pScmdline, int iCmdshow)
{
	NemoGame game;
	if (game.init())
	{
		return game.run();
	}
	return -1;
}