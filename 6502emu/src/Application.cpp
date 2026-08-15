#include "Application.h"

Application::Application(int width, int height): window(width, height) {
	
}

Application::~Application() {

}

void Application::run() {
	while (!window.shouldExit()) {
		window.startFrame();
		window.render(emulator);
		if (window.getEmulationMode() == EmulationMode::Automatic) {
			for(int i = 0; i < emulator.emulationSpeed*100; ++i)
				emulator.clock();
		}
		window.endFrame();
	}
}