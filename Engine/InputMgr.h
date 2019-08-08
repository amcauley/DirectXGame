#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <stdint.h>

// See http ://www.rastertek.com/dx11tut13.html for helpful background.

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

// API to communicate with rest of the program.
typedef struct InputApi_
{
  // Processed directions and magnitudes, in range of -1 (reverse) to 1.
  // These might be binary for keyboard but could have intermediate values for some mouse controls.
  float keyUp{ 0 };
  float keyRight{ 0 };
  
  // Rotations
  float yawCw{ 0 };    // Yaw clockwise
  float pitchUp{ 0 };  // Pitch Up

  // Binary presses.
  bool bSpace{ false };
  bool bSpaceDown{ false };  // Space bar just got pressed.
  bool bSprint{ false };

} InputApi;

class InputMgr
{
private:
  IDirectInput8* m_pDirectInput;
  IDirectInputDevice8* m_pKeyboard;

  // Key enum mapping: https://resources.bisimulations.com/wiki/DIK_KeyCodes
  static const unsigned int KEYBOARD_STATE_ENTRIES = 256;
  unsigned char m_keyboardState[KEYBOARD_STATE_ENTRIES];

public:
  bool init(HINSTANCE hinst, HWND hwnd);
  bool release();

  bool readKeyboard();

  bool getUpdate(InputApi &inputUpdate);
};

#endif