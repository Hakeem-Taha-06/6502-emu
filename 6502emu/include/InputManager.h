#pragma once

class InputManager {
public:

	void setKeyState(int key, bool isPressed) {
		if (key >= 0 && key < 1024)
			keys[key] = isPressed;
	}

	bool isKeyPressed(int key) const {
		if (key >= 0 && key < 1024)
			return keys[key];
		return false;
	}
private:
	bool keys[1024];
};