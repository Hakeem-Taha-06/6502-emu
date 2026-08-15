#pragma once

#include "Emulator.h"
#include "Window.h"

class Application
{
public:
	Application(int width, int height);
	~Application();

	void run();

private:

	Emulator emulator;
	Window window;
};

