#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <thread>

int main()
{
	CreateMutexA(0, FALSE, "SPRINTMOD");
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;
	FreeConsole();

	char* buf = nullptr;
	size_t sz = 0, pos = 0;
	std::string path;

	if (_dupenv_s(&buf, &sz, "LOCALAPPDATA") == 0 && buf != nullptr)
	{
		path = buf;
		free(buf);
	}

	path += "\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe";
	path += "\\LocalState\\games\\com.mojang\\minecraftpe";

	std::string del = ":", key;
	std::ifstream options(path + "\\options.txt");

	int forward = 0, sprint = 0, delay = 250;

	if (options.is_open()) {
		while (getline(options, key))
		{		
			if (key.find("keyboard_type_0_key.forward") != std::string::npos) {
				pos = key.find(del) + 1;
				forward = stoi(key.substr(pos));
			}

			if (key.find("keyboard_type_0_key.sprint") != std::string::npos) {
				pos = key.find(del) + 1;
				sprint = stoi(key.substr(pos));
			}
		}
		options.close();
	}
	else return 0;

	INPUT ip[2] = { 0, 0 };
	ip[0].type = INPUT_KEYBOARD; ip[0].ki.wVk = sprint;
	ip[1].type = INPUT_KEYBOARD; ip[1].ki.wVk = sprint;
	ip[1].ki.dwFlags = KEYEVENTF_KEYUP;

	do {
		if (GetForegroundWindow() == FindWindow(NULL, "Minecraft"))
		{
			CURSORINFO cursor = { sizeof(cursor) };
			if (GetCursorInfo(&cursor)) {
				if (cursor.flags == 0 && GetAsyncKeyState(forward))
				{
					SendInput(1, &ip[0], sizeof(INPUT));
					std::this_thread::sleep_for(std::chrono::milliseconds(delay / 2));
					SendInput(1, &ip[1], sizeof(INPUT));
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	} while (true);

	return 0;
}
