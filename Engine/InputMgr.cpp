#include "InputMgr.h"
#include "Logger.h"
#include "Util.h"


bool InputMgr::init(HINSTANCE hinst, HWND hwnd, uint32_t width, uint32_t height)
{
  m_width = width;
  m_height = height;

  if (HR_FAILED(DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL)))
  {
    LOGE("Failed DirectInput8 creation");
    return false;
  }

  if (HR_FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL)))
  {
    LOGE("Failed DirectInput keyboard creation");
    return false;
  }

  if (HR_FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
  {
    LOGE("Failed to set keyboard data format");
    return false;
  }

  // Set the cooperative level of the keyboard to not share with other programs.
  if(HR_FAILED(m_pKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
  {
    LOGE("Failed to set keyboard coorperative level");
    return false;
  }

  // Now acquire the keyboard.
  if(HR_FAILED(m_pKeyboard->Acquire()))
  {
    LOGE("Failed to acquire keyboard");
    return false;
  }

  return true;
}


bool InputMgr::readKeyboard()
{
  if (!m_pKeyboard)
  {
    LOGW("NULL keyboard ptr");
    return false;
  }

  HRESULT result;
  result = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
  if (FAILED(result))
  {
    // If the keyboard lost focus or was not acquired then try to get control back for next time.
    if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
    {
      // Clear old keyboard info.
      memset(m_keyboardState, 0, sizeof(m_keyboardState));
      m_pKeyboard->Acquire();
    }
    else
    {
      LOGE("Get keyboard state failed: %u", result);
      return false;
    }
  }

  //for (int i = 0; i < KEYBOARD_STATE_ENTRIES; ++i)
  //{
  //  if (m_keyboardState[i] & 0x80)
  //  {
  //    LOGD("Key %u press", i);
  //  }
  //}

  return true;
}


bool InputMgr::getUpdate(InputApi &inputUpdate)
{
  bool bSuccess = true;
  memset(&inputUpdate, 0, sizeof(inputUpdate));

  // Keyboard updates.
  bSuccess = readKeyboard();
  
  if (m_keyboardState[DIK_SPACE] & 0x80)
  {
    inputUpdate.bSpace = true;
  }

  bool bKeyUp     = (0x80 & m_keyboardState[DIK_UP]) ||
                    (0x80 & m_keyboardState[DIK_W]);
  bool bKeyDown   = (0x80 & m_keyboardState[DIK_DOWN]) ||
                    (0x80 & m_keyboardState[DIK_S]);
  bool bKeyRight  = (0x80 & m_keyboardState[DIK_RIGHT]) ||
                    (0x80 & m_keyboardState[DIK_D]);
  bool bKeyLeft   = (0x80 & m_keyboardState[DIK_LEFT]) ||
                    (0x80 & m_keyboardState[DIK_A]);

  if (bKeyUp)
  {
    if (!bKeyDown)
    {
      inputUpdate.keyUp = 1.0;
    }
  }
  else if (bKeyDown)
  {
    inputUpdate.keyUp = -1.0;
  }

  if (bKeyRight)
  {
    if (!bKeyLeft)
    {
      inputUpdate.keyRight = 1.0;
    }
  }
  else if (bKeyLeft)
  {
    inputUpdate.keyRight = -1.0;
  }

  bool bKeyPitchUp    = (0x80 & m_keyboardState[DIK_I]);
  bool bKeyPitchDown  = (0x80 & m_keyboardState[DIK_K]);
  bool bKeyYawCw      = (0x80 & m_keyboardState[DIK_L]);
  bool bKeyYawCcw     = (0x80 & m_keyboardState[DIK_J]);

  if (bKeyPitchUp)
  {
    if (!bKeyPitchDown)
    {
      inputUpdate.pitchUp = 1.0;
    }
  }
  else if (bKeyPitchDown)
  {
    inputUpdate.pitchUp = -1.0;
  }

  if (bKeyYawCw)
  {
    if (!bKeyYawCcw)
    {
      inputUpdate.yawCw = 1.0;
    }
  }
  else if (bKeyYawCcw)
  {
    inputUpdate.yawCw = -1.0;
  }

  inputUpdate.bSprint = 0x80 & m_keyboardState[DIK_LSHIFT];

  //...

  return bSuccess;
}


bool InputMgr::release()
{
  if (m_pKeyboard)
  {
    m_pKeyboard->Unacquire();
    m_pKeyboard->Release();
    m_pKeyboard = NULL;
  }
  RELEASE_NON_NULL(m_pDirectInput);
  return true;
}
