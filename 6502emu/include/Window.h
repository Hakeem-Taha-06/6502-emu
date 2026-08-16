#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>

#include <iostream>
#include <map>
#include <algorithm>

#include "InputManager.h"

// Commodore 64 RGB Palette (Normalized 0.0f to 1.0f)
const float c64_palette[16][3] = {
	{0.0000f, 0.0000f, 0.0000f}, // 00 Black
	{1.0000f, 1.0000f, 1.0000f}, // 01 White
	{0.5333f, 0.0000f, 0.0000f}, // 02 Red
	{0.6667f, 1.0000f, 0.9333f}, // 03 Cyan
	{0.8000f, 0.2667f, 0.8000f}, // 04 Purple
	{0.0000f, 0.8000f, 0.3333f}, // 05 Green
	{0.0000f, 0.0000f, 0.6667f}, // 06 Blue
	{0.9333f, 0.9333f, 0.4667f}, // 07 Yellow
	{0.8667f, 0.5333f, 0.3333f}, // 08 Orange
	{0.4000f, 0.2667f, 0.0000f}, // 09 Brown
	{1.0000f, 0.4667f, 0.4667f}, // 0A Light Red
	{0.2000f, 0.2000f, 0.2000f}, // 0B Dark Grey
	{0.4667f, 0.4667f, 0.4667f}, // 0C Medium Grey
	{0.6667f, 1.0000f, 0.4000f}, // 0D Light Green
	{0.0000f, 0.5333f, 1.0000f}, // 0E Light Blue
	{0.7333f, 0.7333f, 0.7333f}  // 0F Light Grey
};

class Emulator;

enum class EmulationMode {
	Automatic,
	Manual
};

struct WindowContext {
	InputManager* input = nullptr;
	EmulationMode emulationMode = EmulationMode::Manual;
};

class Window
{
public:
	Window(int width, int height);
	~Window();

	void init();
	void startFrame();
	void render(Emulator& emulator);
	void endFrame();

	inline bool shouldExit() const { return glfwWindowShouldClose(m_window); }
	inline EmulationMode getEmulationMode() const { 
		auto context = static_cast<WindowContext*>(glfwGetWindowUserPointer(m_window));
		return context->emulationMode; 
	}
private:
	GLFWwindow* m_window;
	InputManager inputManager;
	WindowContext context;

	int WIDTH = 800, HEIGHT = 600;

	const int MAX_CLOCKS = 100'000'000;

	std::string testRomPath = "src/gamesrc/snake.bin";

	// Memory Viewer
	bool memoryFollowPC = false;
	int testClocks = 0;
	uint16_t targetPC = 0x0000;
	uint16_t searchAddress = 0x0000;
	float targetMemoryYScroll = 0.0f;

	// Disassembly Viewer
	bool disassemblerFollowPC = true;
	uint16_t currentAddr = 0x0000;
	std::map<uint16_t, std::string> disassemblyLines;
	int disassemblyDisplaySize = 20;

	// Screen Display
	unsigned int screenTexture = 0;
	uint16_t screenStartAddr = 0x0200;
	int screenWidth = 32;
	int screenHeight = 32;
	int displayScale = 12;
	int colorShift = 0;

	void renderControlWindow(Emulator& emulator);
	void renderCPUWindow(Emulator& emulator);
	void renderMemoryWindow(Emulator& emulator);
	void renderDisassemblyWindow(Emulator& emulator);
	void renderGameScreenWindow(Emulator& emulator);

	static void keyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void processInput(Emulator& emulator);
};

