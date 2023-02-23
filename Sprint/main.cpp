#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <thread>


int sprint = 0;
int forward = 0;
bool sprinting = false;


void Sprint() {
	INPUT ip[2] = { 0, 0 };
	ip[0].type = INPUT_KEYBOARD; ip[0].ki.wVk = sprint;
	ip[1].type = INPUT_KEYBOARD; ip[1].ki.wVk = sprint;
	ip[1].ki.dwFlags = KEYEVENTF_KEYUP;

	while (true) {
		if (sprinting && GetForegroundWindow() == FindWindow(NULL, "Minecraft")) {
			CURSORINFO cursor = { sizeof(cursor) };
			if (GetCursorInfo(&cursor)) {
				if (cursor.flags == 0)
				{
					SendInput(1, &ip[0], sizeof(INPUT));
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					SendInput(1, &ip[1], sizeof(INPUT));
					sprinting = false;
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		if (wParam == WM_KEYDOWN)
		{
			PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
			if (p->vkCode == forward && !sprinting)
			{
				sprinting = true;
			}
		}
		else if (wParam == WM_KEYUP)
		{
			PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
			if (p->vkCode == forward && sprinting)
			{
				sprinting = false;
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}




int main()
{

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

	std::cout << "Sprint key found: " << (char)sprint;

	std::thread a(Sprint);

	HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

	MSG msg;
	while (!GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hhkLowLevelKybd);


	return 0;
}
