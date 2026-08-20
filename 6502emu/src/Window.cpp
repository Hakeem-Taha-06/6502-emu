#include "Window.h"
#include "Emulator.h"

Window::Window(int width, int height): WIDTH(width), HEIGHT(height), inputManager(), assemblyEditor(), errorDisplay(){
	init();
}

Window::~Window() {
	// ImGui termination (all debug ui stuff can be moved to a separate ui class)
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void Window::init() {
	if (!glfwInit()) {
		std::cerr << "glfw not initialized";
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
	GLFWwindow* w = glfwCreateWindow(WIDTH, HEIGHT, "MOS 6502 Emulator", nullptr, nullptr);
	if (w == nullptr) {
		std::cout << "Failed to create window";

		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	context = WindowContext{&inputManager, EmulationMode::Manual};

	glfwMakeContextCurrent(w);
	glfwSwapInterval(1);
	glfwSetWindowUserPointer(w, &context);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD";
		exit(EXIT_FAILURE);
	}

	std::cout << "GPU: " << glGetString(GL_RENDERER) << '\n';
	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';

	glViewport(0, 0, WIDTH, HEIGHT);

	glfwSetKeyCallback(w, Window::keyEventCallback);

	// Setup Dear ImGui context (all debug ui stuff can be moved to a separate ui class)
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Control
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	
	ImGui::StyleColorsDark();                                 // Configure Style Color
	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);                          // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;                          // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3
	io.ConfigDpiScaleFonts = true;                            // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io.ConfigDpiScaleViewports = true;                        // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
#endif

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(w, true);                    // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init("#version 330");

	// Assembly Code Editor
	assemblyLang = setupAssemblyLanguage();
	assemblyEditor.SetLanguage(&assemblyLang);

	// Error Display
	errorDisplay.SetReadOnlyEnabled(true);
	errorDisplay.SetShowLineNumbersEnabled(false);
	errorDisplay.SetWordWrapEnabled(true);

	m_window = w;

}

void Window::render(Emulator& emulator) {
	
	processInput(emulator);

	//rendering
	glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// IMGUI (all debug ui stuff can be moved to a separate ui class)
	// Create a dockspace that covers the entire viewport
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport());

	renderControlWindow(emulator);
	renderCPUWindow(emulator);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	renderMemoryWindow(emulator);
	ImGui::PopStyleVar(1);

	renderDisassemblyWindow(emulator);
	renderGameScreenWindow(emulator);
	renderTextEditorWindow(emulator);
	renderErrorWindow(emulator);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void Window::renderControlWindow(Emulator& emulator) {
	ImGui::Begin("Control");

	ImGui::InputScalar("Write Address", ImGuiDataType_U16, &romWriteAddr, 0, 0, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::InputText("##RomPath", &testRomPath); 
	ImGui::SameLine();
	if(ImGui::Button("Load binary ROM")) {
		try {
			emulator.load(testRomPath, romWriteAddr);
		}
		catch (std::exception e) {
			std::cerr << e.what() << '\n';
		}
	}

	if (ImGui::Button("Clock Emulator")) {
		emulator.clock();
	}

	ImGui::PushItemWidth(200.0f);
	ImGui::InputInt("##testClocks", &testClocks); 
	ImGui::SameLine();
	if(ImGui::Button("Clock # Times")) {
		
		for (int i = 0; i < testClocks; ++i) {
			emulator.clock();
		}
	}

	ImGui::InputScalar("##targetPC", ImGuiDataType_U16, &targetPC, nullptr, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::SameLine();
	if (ImGui::Button("Clock Until Address")) {
		for (int i = 0; i < MAX_CLOCKS; ++i) {
			emulator.clock();
			if (emulator.bus.cpu.pc >= targetPC) break;
		}
	}

	ImGui::InputInt("Emulation Speed", &emulator.emulationSpeed);

	ImGui::InputInt("Shift Palette", &colorShift);
	ImGui::InputScalar("Display Start Address", ImGuiDataType_U16, &screenStartAddr, 0, 0, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::InputInt("##width", &screenWidth);
	ImGui::SameLine();
	ImGui::InputInt("Screen W/H", &screenHeight);
	ImGui::InputInt("Display Scale", &screenScale);
	if (ImGui::Button("Clear Memory")) {
		emulator.clearMem();
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void Window::renderCPUWindow(Emulator& emulator) {
	ImGui::Begin("CPU");

	ImGui::Text("Accumulator:     0x%02X", emulator.bus.cpu.acc);
	ImGui::Text("X Register:      0x%02X", emulator.bus.cpu.xreg);
	ImGui::Text("Y Register:      0x%02X", emulator.bus.cpu.yreg);
	ImGui::Text("Stack Pointer:   0x%04X", emulator.bus.cpu.stkp);
	ImGui::Text("Program Counter: 0x%04X", emulator.bus.cpu.pc);

	ImGui::Separator();

	ImGui::Text("Carry:             %i", emulator.bus.cpu.getFlag(cpu6502::C));
	ImGui::Text("Zero:              %i", emulator.bus.cpu.getFlag(cpu6502::Z));
	ImGui::Text("Interrupt Disable: %i", emulator.bus.cpu.getFlag(cpu6502::I));
	ImGui::Text("Decimal Mode:      %i", emulator.bus.cpu.getFlag(cpu6502::D));
	ImGui::Text("Break:             %i", emulator.bus.cpu.getFlag(cpu6502::B));
	ImGui::Text("Overflow:          %i", emulator.bus.cpu.getFlag(cpu6502::V));
	ImGui::Text("Negative:          %i", emulator.bus.cpu.getFlag(cpu6502::N));

	ImGui::Text("Total Cycles:      %i", emulator.bus.cpu.totalCycles);
	ImGui::End();
}

void Window::renderMemoryWindow(Emulator& emulator) {
	ImGui::Begin("Memory");

	// setup
	size_t ramSize = 64*1024;
	int numCols = 16;
	int numRows = (int)ramSize / numCols;
	ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;

	ImGui::Checkbox("Follow PC", &memoryFollowPC);

	if (memoryFollowPC) {
		// set target y scroll based on pc row
		int pcRow = emulator.bus.cpu.pc / numCols;
		ImGuiStyle& style = ImGui::GetStyle();
		float rowHeight = ImGui::GetTextLineHeight() + style.CellPadding.y * 2.0f;
		targetMemoryYScroll = pcRow*rowHeight;
	}

	// Memory address search
	bool wantSearch = false;
	ImGui::Text("Search Address");
	ImGui::InputScalar("##address", ImGuiDataType_U16, &searchAddress, nullptr, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::SameLine();
	if (ImGui::Button("GOTO") && !wantSearch) {
		wantSearch = true;
		memoryFollowPC = false;
		int searchRow = searchAddress / numCols;
		ImGuiStyle& style = ImGui::GetStyle();
		float rowHeight = ImGui::GetTextLineHeight() + style.CellPadding.y * 2.0f;
		targetMemoryYScroll = searchRow * rowHeight;

	}

	if (ImGui::BeginTable("Ram", numCols + 1, flags)) {

		// set fixed width for columns
		ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 60.0f);
		for (int i = 0; i < numCols; i++) {
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 32.0f);
		}

		if (wantSearch) {
			ImGui::SetScrollY(targetMemoryYScroll);
			wantSearch = false;
		}

		if (memoryFollowPC) {
			ImGui::SetScrollY(targetMemoryYScroll);
		}

		ImGuiListClipper clipper;
		clipper.Begin(numRows);

		while (clipper.Step()) {
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%04X", row * numCols);

				for (int col = 0; col < numCols; ++col) {
					ImGui::TableSetColumnIndex(col + 1);

					uint16_t byteIdx = row * numCols + col;
					ImGui::PushID((int)byteIdx);
					ImGui::PushItemWidth(-FLT_MIN);

					uint8_t data = emulator.bus.cpuRead(byteIdx);

					bool isPcIdx = emulator.bus.cpu.pc == byteIdx;
					bool isStkIdx = emulator.bus.cpu.stkp + 0x100 == byteIdx;
					
					ImVec4 textColor{1.0f, 1.0f, 1.0f, 1.0f};

					if (isPcIdx) 
						textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

					if(isStkIdx)
						textColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);

					if (byteIdx < ramSize) {
						ImGui::TextColored(textColor, "%02X", data);
					}

					ImGui::PopItemWidth();
					ImGui::PopID();
				}
			}
		}

		ImGui::EndTable();
	}
	ImGui::End();
}


void Window::renderDisassemblyWindow(Emulator& emulator) {
	ImGui::Begin("Disassembly");

	ImGui::Checkbox("Follow PC", &disassemblerFollowPC);
	ImGui::SameLine();
	if (ImGui::Button("<")) {
		disassemblerCurrentAddr--;
		disassemblerFollowPC = false;
	}
	ImGui::SameLine();
	if (ImGui::Button(">")) {
		disassemblerCurrentAddr++;
		disassemblerFollowPC = false;
	}

	if (ImGui::InputScalar("##disassembleraddr", ImGuiDataType_U16, &disassemblerCurrentAddr, 0, 0, "%04X", ImGuiInputTextFlags_CharsHexadecimal)) {
		disassemblerFollowPC = false;
	}

	if (disassemblerFollowPC) disassemblerCurrentAddr = emulator.bus.cpu.pc;


	uint16_t startAddr = disassemblerCurrentAddr;
	uint16_t endAddr = disassemblerCurrentAddr + disassemblyDisplaySize;

	disassemblyLines = emulator.bus.cpu.disassemble(startAddr, endAddr);

	if (ImGui::BeginTable("##disassembly", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
		
		//ImGuiListClipper clipper;
		//clipper.Begin(disassemblyLines.size());
		//
		//while (clipper.Step()) {
		//	for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
		//
		//	}
		//}

		int i = 0;
		for (auto& [addr, instr] : disassemblyLines) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			if (addr == emulator.bus.cpu.pc) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", instr.c_str());
			}
			else {
				ImGui::Text("%s", instr.c_str());
			}
			++i;
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

void Window::renderGameScreenWindow(Emulator& emulator) {
	ImGui::Begin("Screen Display");

	std::vector<float> screen(screenWidth*screenHeight*3);
	int offset = 0;
	for (int i = 0; i < screenWidth * screenHeight * 3; ++i) {
		uint8_t val = emulator.bus.cpuRead(screenStartAddr + offset);
		offset++;
		screen[i++] = c64_palette[(val+colorShift)%15][0];
		screen[i++] = c64_palette[(val+colorShift)%15][1];
		screen[i]   = c64_palette[(val+colorShift)%15][2];
	}

	if(screenTexture == 0)
		glGenTextures(1, &screenTexture);

	glBindTexture(GL_TEXTURE_2D, screenTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, screen.data());

	ImTextureID imTexture = (ImTextureID)(intptr_t)screenTexture;
	ImGui::GetWindowDrawList()->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest, nullptr);
	ImGui::Image(imTexture, ImVec2((float)(screenWidth * screenScale), (float)(screenHeight * screenScale)));
	ImGui::GetWindowDrawList()->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerLinear, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
	ImGui::End();
}

void Window::renderTextEditorWindow(Emulator& emulator) {
	ImGui::Begin("Assembly Editor");

	// Binary File Management
	ImGui::InputText("##assemblycodepath", &assemblyCodePath);
	ImGui::SameLine();
	ImGui::PushID(0);
	if (ImGui::Button("Browse")) {
		const char* filterPatters[1] = { "*.asm" };
		const char* path = tinyfd_openFileDialog(
			"Choose .bin File",
			"",
			1,
			filterPatters,
			".asm",
			0
		);
		if(path)
			assemblyCodePath = path;
	}
	ImGui::PopID();
	ImGui::SameLine();
	if (ImGui::Button("Load assembly code")) {
		try {
			assemblyCode = readFile(assemblyCodePath);
			assemblyEditor.SetText(assemblyCode);
		}
		catch(std::exception e){
			std::cerr << "Error Loading File { " << assemblyCodePath << " } : " << e.what() << '\n';
		}
	}

	// Write Offset
	ImGui::InputScalar("Write Address", ImGuiDataType_U16, &assemblerWriteAddr, 0, 0, "%04X", ImGuiInputTextFlags_CharsHexadecimal);

	// Assembler Management
	ImGui::InputText("##assemblerpath", &assemblerPath);
	ImGui::SameLine();
	ImGui::PushID(1);
	if (ImGui::Button("Browse")) {
		const char* filterPatters[1] = { "*.exe" };
		const char* path = tinyfd_openFileDialog(
			"Choose Assembler Executable",
			"",
			1,
			filterPatters,
			".exe",
			0
		);
		if(path)
			assemblerPath = path;
	}
	ImGui::PopID();

	ImGui::SameLine();
	if (ImGui::Button("Assemble")) {
		std::ofstream("prg.asm") << assemblyEditor.GetText();

		std::string cmd;
		switch (assembler) {
			//case Assembler::CA65:
			//	cmd = "";
			//	break;
			
			case Assembler::VASM:
			default: // VASM
				cmd = "\"" + assemblerPath + "\" -Fbin -o prg.bin -L prg.lst prg.asm 2> stderr.txt";
		}
		
		try{
			executeTerminalCommand(cmd.c_str()); 
			emulator.load("prg.bin", assemblerWriteAddr);
			std::cout << "Successfully Assembled and Loaded Program" << '\n';
			errorDisplay.SetText("Success");
		}
		catch(std::exception e){
			std::cerr << e.what() << '\n';
			if (std::filesystem::exists("stderr.txt"))
				errorDisplay.SetText(readFile("stderr.txt"));
		}

		emulator.reset();
		removeFile("prg.asm");
		removeFile("prg.bin");
		removeFile("prg.lst");
		removeFile("stderr.txt");
	}

	assemblyEditor.Render("Assembly Editor");
	ImGui::End();
}

void Window::renderErrorWindow(Emulator& emulator) {
	ImGui::Begin("Errors");

	errorDisplay.Render("Errors");
	ImGui::End();
}

void Window::startFrame() {
	glfwPollEvents();

	// Start the Dear ImGui frame (all debug ui stuff can be moved to a separate ui class)
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::endFrame() {
	glfwSwapBuffers(m_window);
}

void Window::keyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	ImGuiIO& io = ImGui::GetIO();
	if (io.WantTextInput) 
		return;
	

	auto ctx = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
	
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (ctx->emulationMode == EmulationMode::Manual)
			ctx->emulationMode = EmulationMode::Automatic;
		else if (ctx->emulationMode == EmulationMode::Automatic)
			ctx->emulationMode = EmulationMode::Manual;
	}
	
	if (action == GLFW_PRESS) {
		ctx->input->setKeyState(key, true);
	}
	else if (action == GLFW_RELEASE) {
		ctx->input->setKeyState(key, false);
	}
}

void Window::processInput(Emulator& emulator) {
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantTextInput)
		return;

	auto ctx = (WindowContext*)glfwGetWindowUserPointer(m_window);
	if (ctx->input->isKeyPressed(GLFW_KEY_W)) {
		emulator.bus.cpuWrite(0x00FF, 0x77);
	}
	if (ctx->input->isKeyPressed(GLFW_KEY_D)) {
		emulator.bus.cpuWrite(0x00FF, 0x64);
	}
	if (ctx->input->isKeyPressed(GLFW_KEY_S)) {
		emulator.bus.cpuWrite(0x00FF, 0x73);
	}
	if (ctx->input->isKeyPressed(GLFW_KEY_A)) {
		emulator.bus.cpuWrite(0x00FF, 0x61);
	}
	if (ctx->input->isKeyPressed(GLFW_KEY_T)) {
		screenStartAddr += screenWidth;
	}
	if (ctx->input->isKeyPressed(GLFW_KEY_Y)) {
		screenStartAddr-= screenWidth;
	}
}	

std::string Window::readFile(std::string path) {
	std::ifstream file(path);
	
	if (!file)
		throw std::runtime_error("File not open");

	std::string contents{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	return contents;
}

void Window::removeFile(std::string fileName) {
	try {
		if (std::filesystem::remove(fileName)) {
			std::cout << "File '" << fileName << "'" << " Removed Successfully" << '\n';
		}
		else {
			std::cout << "File '" << fileName << "'" << " not Found" << '\n';
		}
	}
	catch (std::exception e) {
		std::cerr << "Error Deleting File '" << fileName << "'" << '\n';
	}
}

void Window::executeTerminalCommand(const char* cmd){
	int result = std::system(cmd);

	if (result != 0) {
		std::string error = "Command {";
		error += cmd;
		error += "} Failed Execution With Exit Code ";
		error += result;
		throw std::runtime_error(error);
	}
}

TextEditor::Language Window::setupAssemblyLanguage() {
	TextEditor::Language assemblyLang;

	assemblyLang.singleLineComment = ";";

	return assemblyLang;
}