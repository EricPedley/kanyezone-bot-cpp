#ifndef INPUT
#include <windows.h>
#endif
// int main() {
//     cout << "pressing A\n";
//     INPUT Input = { 0 };
//     Input.type = INPUT_KEYBOARD;
//     Input.ki.wVk = VkKeyScanA('a');
//     SendInput(1, &Input, sizeof(Input));
//     ZeroMemory(&Input, sizeof(Input));
//     Input.ki.dwFlags = KEYEVENTF_KEYUP;
//     SendInput(1, &Input, sizeof(Input));
// }

void pressKey(char key) {//press and don't release
    INPUT Input = {0};
    Input.type=INPUT_KEYBOARD;
    Input.ki.wVk = VkKeyScanA(key);
    SendInput(1,&Input,sizeof(Input));
}

void releaseKey(char key) {//release key
    INPUT Input = {0};
    Input.type=INPUT_KEYBOARD;
    Input.ki.wVk = VkKeyScanA(key);
    Input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &Input, sizeof(Input));
}

void tapKey(char key) {//press and release key
    INPUT Input = {0};
    Input.type = INPUT_KEYBOARD;
    Input.ki.wVk = VkKeyScanA(key);
    SendInput(1, &Input, sizeof(Input));
    ZeroMemory(&Input, sizeof(Input));
    Input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &Input, sizeof(Input));
}